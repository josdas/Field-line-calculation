#include "quadtree.h"

Quadtree::node::node(node* left, node* right, box limit, int h, int full_empty) :
	left(left),
	right(right),
	limit(limit),
	h(h),
	full_empty(full_empty) { }

Quadtree::node::node(node* left, node* right, box limit, int h):
	left(left),
	right(right),
	limit(limit),
	h(h) {
	full_empty = left->full_empty && right->full_empty;
}

int Quadtree::test_for_in_out(box limit) {
	bool ok[4];
	for (auto obj : objects) {
		ok[obj.cross(limit)] = true;
	}
	if (ok[1]) {
		return 1;
	}
	if (ok[2] | ok[3]) {
		return 2;
	}
	return 0;
}

void Quadtree::add_zone(box limit) {
	zones.push_back(limit);
}

Quadtree::node* Quadtree::dfs(box limit, int h) {
	int temp = test_for_in_out(limit);
	if (temp == 1) {
		add_zone(limit);
		return new node(
			nullptr,
			nullptr,
			limit,
			h,
			1
		);
	}
	if (temp == 0 || h > MAX_H) {
		return new node(
			nullptr,
			nullptr,
			limit,
			h,
			2
		);
	}
	Point s = (limit.first + limit.second) / 2;
	box a = limit;
	box b = limit;
	int cur_d = h % 3;
	a.first[cur_d] = s[cur_d];
	b.second[cur_d] = s[cur_d];
	auto cur_node = new node(
		dfs(a, h + 1),
		dfs(b, h + 1),
		limit,
		h
	);
	return cur_node;
}

int Quadtree::get(Point t, node* cur) {
	assert(cur != nullptr);
	if (!cur->limit.contains(t)) {
		return 0;
	}
	if (cur->full_empty > 0) {
		return cur->full_empty;
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

Quadtree::Quadtree(std::vector<object> const& objects_, box limit): objects(objects_) {
	root = dfs(limit, 0);
}

Quadtree::~Quadtree() {
	clear();
}

bool Quadtree::is_empty_point(Point p) {
	return get(p, root) == 2;
}

std::vector<box> Quadtree::get_zones() const {
	return zones;
}
