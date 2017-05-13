#include <iostream>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <vector>

using namespace std;

const float eps = 1e-3f;

bool is_zero(float x) {
	return abs(x) < eps;
}

struct Point {
	union {
		struct {
			float x, y, z;
		};
		float data[3];
	};
	Point(float _x, float _y, float _z) : x(_x), y(_y), z(_z) { }
	Point() : Point(0, 0, 0) { }
	Point(Point const& b) : Point(b.x, b.y, b.z) { }

	friend Point operator+(Point a, Point b) {
		return Point(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	friend Point operator-(Point a, Point b) {
		return Point(a.x - b.x, a.y - b.y, a.z - b.z);
	}
	
	float& operator[](int t) {
		return data[t];
	}

	float operator[](int t) const {
		return data[t];
	}

	Point operator-=(Point b) {
		return *this = *this - b;
	}

	friend bool operator<(Point a, Point b) {
		for(int i = 0; i < 3; i++) {
			if(!is_zero(a[i] - b[i])) {
				return a[i] < b[i];
			}
		}
		return false;
	}

	friend bool operator==(Point a, Point b) {
		return is_zero(a.x - b.x) && is_zero(a.y - b.y) && is_zero(a.z - b.z);
	}

	friend bool operator<=(Point a, Point b) {
		return a < b || a == b;
	}

	Point operator/(float c) const {
		return Point(x / c, y / c, z / c);
	}
};

Point cross_product(Point a, Point b) { // (a2b3  -   a3b2,     a3b1   -   a1b3,     a1b2   -   a2b1)
	return Point(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

float dot_product(Point a, Point b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}


struct Triangle {
	union {
		struct {
			Point a, b, c;
		};
		Point points[3];
	};

	Triangle(Point a, Point b, Point c) {
		points[0] = a;
		points[1] = b;
		points[2] = c;
	}
	
	Triangle(Triangle const& b) {
		points[0] = b.points[0];
		points[1] = b.points[1];
		points[2] = b.points[2];
	}

	typedef Point* iterator;
	iterator begin() {
		return points;
	}
	iterator end() {
		return points + 3;
	}
	Point operator[](int x) {
		assert(x < 3);
		return points[x];
	}
};

struct Interval {
	float min;
	float max;
};

Interval GetInterval(Triangle triangle, Point axis) {
	Interval result;

	result.min = dot_product(axis, triangle.points[0]);
	result.max = result.min;
	for (int i = 1; i < 3; ++i) {
		float value = dot_product(axis, triangle.points[i]);
		result.min = fminf(result.min, value);
		result.max = fmaxf(result.max, value);
	}

	return result;
}

bool OverlapOnAxis(Triangle t1, Triangle t2, Point axis) {
	Interval a = GetInterval(t1, axis);
	Interval b = GetInterval(t2, axis);
	return ((b.min <= a.max) && (a.min <= b.max));
}

bool cross_triangle_triangle(Triangle t1, Triangle t2) {
	Point t1_f0 = t1.b - t1.a; // Edge 0
	Point t1_f1 = t1.c - t1.b; // Edge 1
	Point t1_f2 = t1.a - t1.c; // Edge 2

	Point t2_f0 = t2.b - t2.a; // Edge 0
	Point t2_f1 = t2.c - t2.b; // Edge 1
	Point t2_f2 = t2.a - t2.c; // Edge 2

	Point axisToTest[] = {
		// Triangle 1, Normal
		cross_product(t1_f0, t1_f1),
		// Triangle 2, Normal
		cross_product(t2_f0, t2_f1),

		// Cross Product of edges
		cross_product(t2_f0, t1_f0),
		cross_product(t2_f0, t1_f1),
		cross_product(t2_f0, t1_f2),

		cross_product(t2_f1, t1_f0),
		cross_product(t2_f1, t1_f1),
		cross_product(t2_f1, t1_f2),

		cross_product(t2_f2, t1_f0),
		cross_product(t2_f2, t1_f1),
		cross_product(t2_f2, t1_f2),
	};

	for (int i = 0; i < 11; ++i) {
		if (!OverlapOnAxis(t1, t2, axisToTest[i])) {
			return false; // Seperating axis found
		}
	}
	return true; // Seperating axis not found
}

struct box {
	Point first, second;

	bool contains(Point x) const {
		return first <= x && x <= second;
	}

	vector<Point> get_points() const {
		vector<Point> temp;
		for(int i = 0; i < 8; i++) {
			Point p;
			for(int j = 0; j < 3; j++) {
				if(i & (1 << j)) {
					p[j] = first[j];
				}
				else {
					p[j] = second[j];
				}
			}
			temp.push_back(p);
		}
		return temp;
	}
};

float tetrahedron_volume(Point a, Point b, Point c, Point d) {
	b -= d, c -= d, a -= d;
	return dot_product(a, cross_product(b, d)) / 6;
}

float tetrahedron_volume(Point a, Triangle tr) {
	return tetrahedron_volume(a, tr[0], tr[1], tr[2]);
}

class object {
	vector<Triangle> polygones;
	vector<Point> points;
	float real_volume;
public:
	typedef vector<Triangle>::iterator iterator;
	iterator begin() {
		return polygones.begin();
	}
	iterator end() {
		return polygones.end();
	}

	void init() {
		real_volume = 0;
		for (Triangle v : polygones) {
			real_volume += tetrahedron_volume(Point(), v);
			for (Point u : v) {
				points.push_back(u);
			}
		}
		real_volume = abs(real_volume);
		sort(points.begin(), points.end());
		points.resize(unique(points.begin(), points.end()) - points.begin());
	}

	object(const vector<Triangle>& trianguals)
		: polygones(trianguals) {
		init();
	}

	object(box trianguals) {
		for (int i = 0; i < 8; i++) {
			Point p;
			for (int j = 0; j < 3; j++) {
				if (i & (1 << j)) {
					p[j] = trianguals.first[j];
				}
				else {
					p[j] = trianguals.second[j];
				}
			}
			for(int j = 0; j < 3; j++) {
				Point a = p;
				Point b = p;
				a[j] = trianguals.first[j];
				b[(j + 2) % 3] = trianguals.second[(j + 2) % 3];
				polygones.push_back(Triangle(a, p, b));
			}
		}
		init();
	}

	bool contains(Point p) { // calculate volume
		float temp = 0;
		for(Triangle v : polygones) {
			temp += abs(tetrahedron_volume(p, v));
		}
		return is_zero(temp - real_volume);
	}

	int cross(box limit) { // 0 - empty, 1 - obj in limit, 2 - limit in obj, 3 - limit and obj have intersection
		vector<Point> limit_points = limit.get_points();
		object limit_obj = limit;
		bool in = true;
		for (Point v : limit_points) {
			if (!contains(v)) {
				in = false;
			}
		}
		if(in) {
			return 1;
		}
		bool out = true;
		for(Point tpoint : points) {
			if(!limit_obj.contains(tpoint)) {
				out = false;			
			}
		}
		if(out) {
			return 2;
		}
		for(Triangle v : limit_obj) {
			for(Triangle u : polygones) {
				if(cross_triangle_triangle(v, u)) {
					return 3;
				}
			}
		}
		return 0;
	}
};

class Quadtree {
	const int MAX_H = 10;
	struct node {
		node(node* left, node* right, const box& limit, int h, int full_empty)
			: left(left),
			  right(right),
			  limit(limit),
			  h(h),
			  full_empty(full_empty) { }

		node(node* left, node* right, const box& limit, int h)
			: left(left),
			  right(right),
			  limit(limit),
			  h(h) {
			full_empty = left->full_empty && right->full_empty;
		}

		node* left;
		node* right;
		box limit;
		int h;
		int full_empty; // 1 - full, 2 - empty
	};

	vector<object> objects;
	vector<box> zones;
	node* root;

	int test_for_in_out(box limit) {
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

	void add_zone(box limit) {
		zones.push_back(limit);
	}

	node* dfs(box limit, int h) {
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

	int get(Point t, node* cur) {
		assert(cur != nullptr);
		if (!cur->limit.contains(t)) {
			return 0;
		}
		if (cur->full_empty > 0) {
			return cur->full_empty;
		}
		return get(t, cur->left) | get(t, cur->right);
	}

	void clear_dfs(node* cur) {
		if(cur != nullptr) {
			clear_dfs(cur->left);
			clear_dfs(cur->right);
			delete cur;
		}
	}

public:
	void clear() {
		clear_dfs(root);
		root = nullptr;
	}
	Quadtree(vector<object> const& objects_, box limit)
		: objects(objects_) {
		root = dfs(limit, 0);
	}
	~Quadtree() {
		clear();
	}
	bool is_empty_point(Point p) {
		return get(p, root) == 2;
	}
	vector<box> get_zones() const {
		return zones;
	}
};

int main() {
	
}