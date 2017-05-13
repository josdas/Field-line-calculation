#pragma once
#include <vector>
#include "geometry.h"

class Quadtree {
	struct node {
		node(node* left, node* right, Box limit, int h, int full_empty);
		node(node* left, node* right, Box limit, int h);
		node(Box limit, int h, int full_empty);

		node* left;
		node* right;
		Box limit;
		int h;
		int full_empty; // 1 - full, 2 - empty
	};

	std::vector<Object> objects;
	std::vector<Box> zones;
	node* root;

	int test_for_in_out(Box limit);
	void add_zone(Box limit);

	node* dfs(Box limit, int h);
	int get(Point t, node* cur);
	void clear_dfs(node* cur);
public:
	Quadtree(std::vector<Object> const& objects_, Box limit);
	~Quadtree();
	void clear();

	bool is_empty_point(Point p);

	std::vector<Box> get_zones() const;
};
