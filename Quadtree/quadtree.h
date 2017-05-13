#pragma once
#include <vector>
#include <cassert>
#include "geometry.h"

class Quadtree {
	const int MAX_H = 10;

	struct node {
		node(node* left, node* right, box limit, int h, int full_empty);
		node(node* left, node* right, box limit, int h);

		node* left;
		node* right;
		box limit;
		int h;
		int full_empty; // 1 - full, 2 - empty
	};

	std::vector<object> objects;
	std::vector<box> zones;
	node* root;

	int test_for_in_out(box limit);
	void add_zone(box limit);

	node* dfs(box limit, int h);
	int get(Point t, node* cur);
	void clear_dfs(node* cur);
public:
	Quadtree(std::vector<object> const& objects_, box limit);
	~Quadtree();
	void clear();

	bool is_empty_point(Point p);

	std::vector<box> get_zones() const;
};
