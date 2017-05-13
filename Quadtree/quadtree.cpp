#include "quadtree.h"
#include <cassert>
#include <iostream>
const int MAX_H = 11;

Quadtree::node::node(node* left, node* right, Box limit, int h, NodeType full_empty) :
	left(left),
	right(right),
	limit(limit),
	h(h),
	type(full_empty) { }


NodeType Quadtree::get_type(node* v) {
	if (v == nullptr) {
		return EMPTY_NODE;
	}
	return v->type;
}

Quadtree::node::node(node* left, node* right, Box limit, int h) :
	left(left),
	right(right),
	limit(limit),
	h(h) {
	type = static_cast<NodeType>(get_type(left) | get_type(right));
}

Quadtree::node::node(Box limit, int h, NodeType full_empty) :
	left(nullptr),
	right(nullptr),
	limit(limit), 
	h(h),
	type(full_empty) { }

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

void Quadtree::add_zone(Box limit) {
	zones.push_back(limit);
}

Quadtree::node* Quadtree::dfs(Box limit, int h) {
	NodeType temp = test_for_in_out(limit);
	if (temp == FULL_NODE) {
		add_zone(limit);
		return new node(
			limit,
			h,
			FULL_NODE
		);
	}
	if (temp == EMPTY_NODE || h > MAX_H) {
		return nullptr;
	}
	Point s = (limit.first + limit.second) / 2;
	Box a = limit;
	Box b = limit;
	int cur_d = h % 3;
	a.first[cur_d] = s[cur_d];
	b.second[cur_d] = s[cur_d];
	
	auto left = dfs(a, h + 1);
	auto right = dfs(b, h + 1);
	if(get_type(left) == get_type(right) 
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

bool Quadtree::get(Point t, node* cur) {
	if(cur == nullptr) {
		return true;
	}
	if (!cur->limit.contains(t)) {
		return false;
	}
	if (cur->type != NO_EMPTY_NODE) {
		return cur->type == EMPTY;
	}
	return get(t, cur->left) | get(t, cur->right);
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

Quadtree::Quadtree(std::vector<Object> const& objects_, Box limit): objects(objects_) {
	root = dfs(limit, 0);
}

Quadtree::~Quadtree() {
	clear();
}

bool Quadtree::is_empty_point(Point p) {
	return get(p, root);
}

std::vector<Box> Quadtree::get_zones() const {
	return zones;
}
