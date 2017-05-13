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
		node(node* left, node* right, Box limit, int height, NodeType full_empty);
		node(node* left, node* right, Box limit, int height);
		node(Box limit, int height, NodeType full_empty, float charge);

		node* left;
		node* right;
		Box limit;
		int height;
		NodeType type;
		float charge;
	};

	std::vector<PObject> objects;
	std::vector<Box> zones;
	node* root;

	float Quadtree::get_sum_charge(Box limit);
	NodeType test_for_in_out(Box limit);
	void add_zone(Box limit);

	node* dfs(Box limit, int height);
	node* get(Point t, node* cur, int height) const;
	void clear_dfs(node* cur);

	static float get_charge(node* v);
	static NodeType get_type(node* v);
public:
	Quadtree(std::vector<PObject> const& objects_, Box limit);
	~Quadtree();
	void clear();

	bool is_empty_point(Point p) const;
	float get_charge(Point p) const;

	std::vector<Box> get_zones() const;
};
