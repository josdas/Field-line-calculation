#include "quadtree.h"
#include <cassert>
#include <iostream>
const int MAX_H = 16;

Quadtree::node::node(node* left, node* right, Box limit, int h, NodeType full_empty) :
	left(left),
	right(right),
	limit(limit),
	h(h),
	type(full_empty),
	charge(0) { }


NodeType Quadtree::get_type(node* v) {
	if (v == nullptr) {
		return EMPTY_NODE;
	}
	return v->type;
}

float Quadtree::get_charge(node* v) {
	if (v == nullptr) {
		return 0;
	}
	return v->charge;

}

Quadtree::node::node(node* left, node* right, Box limit, int h) :
	left(left),
	right(right),
	limit(limit),
	h(h) {
	type = static_cast<NodeType>(get_type(left) | get_type(right));
	charge = get_charge(left) + get_charge(right);
}

Quadtree::node::node(Box limit, int h, NodeType full_empty, float charge) :
	left(nullptr),
	right(nullptr),
	limit(limit),
	h(h),
	type(full_empty),
	charge(charge) { }

NodeType Quadtree::test_for_in_out(Box limit) {
	bool ok[4] = {};
	for (auto obj : objects) {
		ok[obj.cross(limit)] = true;
	}
	if (ok[LIMIT_IN_OBJ]) {
		return FULL_NODE;
	}
	if (ok[OBJ_IN_LIMIT] | ok[INTERSECTION]) {
		return NO_EMPTY_NODE;
	}
	return EMPTY_NODE;
}

float Quadtree::get_sum_charge(Box limit) {
	float result = 0;
	for (auto obj : objects) {
		auto t = obj.cross(limit);
		if (t != EMPTY) {
			result += obj.get_charge();
		}
	}
	return result;
}

void Quadtree::add_zone(Box limit) {
	zones.push_back(limit);
}

std::pair<Box, Box> divide_box(int h, Box limit) {
	Point s = (limit.first + limit.second) / 2;
	Box a = limit;
	Box b = limit;
	int cur_d = h % 3;
	a.first[cur_d] = s[cur_d];
	b.second[cur_d] = s[cur_d];
	return std::make_pair(a, b);
}

Quadtree::node* Quadtree::dfs(Box limit, int h) {
	NodeType temp = test_for_in_out(limit);
	if (temp == FULL_NODE) {
		add_zone(limit);
		float sum = get_sum_charge(limit);
		return new node(
			limit,
			h,
			FULL_NODE,
			sum
		);
	}
	if (temp == EMPTY_NODE || h > MAX_H) {
		return nullptr;
	}
	auto boxs = divide_box(h, limit);
	auto left = dfs(boxs.first, h + 1);
	auto right = dfs(boxs.second, h + 1);
	if (get_type(left) == get_type(right)
		&& get_type(left) == EMPTY_NODE) {
		return nullptr;
	}
	return new node(
		left,
		right,
		limit,
		h
	);
}

Quadtree::node* Quadtree::get(Point t, node* cur, int h) const {
	if (cur == nullptr) {
		return nullptr;
	}
	if (cur->type != NO_EMPTY_NODE) {
		return cur;
	}
	auto boxs = divide_box(h, cur->limit);
	if (boxs.first.contains(t)) {
		return get(t, cur->left, h + 1);
	}
	return get(t, cur->right, h + 1);
}

void Quadtree::clear_dfs(node* cur) {
	if (cur != nullptr) {
		clear_dfs(cur->left);
		clear_dfs(cur->right);
		delete cur;
	}
}

void Quadtree::clear() {
	clear_dfs(root);
	root = nullptr;
}

Quadtree::Quadtree(std::vector<PObject> const& objects_, Box limit): objects(objects_) {
	root = dfs(limit, 0);
}

Quadtree::~Quadtree() {
	clear();
}

bool Quadtree::is_empty_point(Point p) const {
	return get_type(get(p, root, 0)) == EMPTY;
}

float Quadtree::get_charge(Point p) const {
	return get_charge(get(p, root, 0));
}

std::vector<Box> Quadtree::get_zones() const {
	return zones;
}
