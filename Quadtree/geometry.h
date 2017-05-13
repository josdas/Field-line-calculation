#pragma once
#include <vector>
#include <algorithm>
#include <cassert>

bool is_zero(float x);

struct Point {
	union {
		struct {
			float x, y, z;
		};
		float data[3];
	};

	Point(float _x, float _y, float _z);
	Point();
	Point(Point const& b) : Point(b.x, b.y, b.z) { }

	friend Point operator+(Point a, Point b);
	friend Point operator-(Point a, Point b);

	float& operator[](int t);
	float operator[](int t) const;

	friend bool operator<(Point a, Point b);
	friend bool operator==(Point a, Point b);
	friend bool operator<=(Point a, Point b);

	Point operator/(float c) const;
	Point operator-=(Point b);
};

Point cross_product(Point a, Point b);

float dot_product(Point a, Point b);


struct Triangle {
	typedef Point* iterator;
	union {
		struct {
			Point a, b, c;
		};

		Point points[3];
	};

	Triangle(Point a, Point b, Point c);
	Triangle(Triangle const& b);

	iterator begin();
	iterator end();
	Point operator[](int x);
};

struct Interval {
	float min;
	float max;
};

Interval GetInterval(Triangle triangle, Point axis);
bool OverlapOnAxis(Triangle t1, Triangle t2, Point axis);
bool cross_triangle_triangle(Triangle t1, Triangle t2);

struct box {
	Point first, second;

	bool contains(Point x) const;
	std::vector<Point> get_points() const;
};

float tetrahedron_volume(Point a, Point b, Point c, Point d);
float tetrahedron_volume(Point a, Triangle tr);

class object {
	std::vector<Triangle> polygones;
	std::vector<Point> points;
	float real_volume;

	void init();
public:
	typedef std::vector<Triangle>::iterator iterator;
	iterator begin();
	iterator end();

	object(const std::vector<Triangle>& trianguals);
	object(box trianguals);

	bool contains(Point p);
	int cross(box limit);
};
