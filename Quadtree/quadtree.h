#pragma once
#include <vector>
#include "geometry.h"

enum NodeType {
	EMPTY_NODE = 1,
	FULL_NODE = 2,
	NO_EMPTY_NODE = 3
};
class Quadtree {
	struct node {
		node(node* left, node* right, Box limit, int h, NodeType full_empty);
		node(node* left, node* right, Box limit, int h);
		node(Box limit, int h, NodeType full_empty);

		node* left;
		node* right;
		Box limit;
		int h;
		NodeType type;
	};

	std::vector<Object> objects;
	std::vector<Box> zones;
	node* root;

	NodeType test_for_in_out(Box limit);
	void add_zone(Box limit);

	node* dfs(Box limit, int h);
	bool get(Point t, node* cur, int h);
	void clear_dfs(node* cur);

	static NodeType get_type(node* v);
public:
	Quadtree(std::vector<Object> const& objects_, Box limit);
	~Quadtree();
	void clear();

	bool is_empty_point(Point p);

	std::vector<Box> get_zones() const;
};
