#include "geometry.h"

const float eps = 1e-3f;

bool is_zero(float x) {
	return abs(x) < eps;
}

Point::Point(float _x, float _y, float _z): x(_x), y(_y), z(_z) { }
Point::Point(): Point(0, 0, 0) { }

float& Point::operator[](int t) {
	return data[t];
}

float Point::operator[](int t) const {
	return data[t];
}

Point Point::operator-=(Point b) {
	return *this = *this - b;
}

Point Point::operator/(float c) const {
	return Point(x / c, y / c, z / c);
}

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

Triangle::Triangle(Point a, Point b, Point c) {
	points[0] = a;
	points[1] = b;
	points[2] = c;
}

Triangle::Triangle(Triangle const& b) {
	points[0] = b.points[0];
	points[1] = b.points[1];
	points[2] = b.points[2];
}

Triangle::iterator Triangle::begin() {
	return points;
}

Triangle::iterator Triangle::end() {
	return points + 3;
}

Point Triangle::operator[](int x) {
	assert(x < 3);
	return points[x];
}

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

bool box::contains(Point x) const {
	return first <= x && x <= second;
}

std::vector<Point> box::get_points() const {
	std::vector<Point> temp;
	for (int i = 0; i < 8; i++) {
		Point p;
		for (int j = 0; j < 3; j++) {
			if (i & (1 << j)) {
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

float tetrahedron_volume(Point a, Point b, Point c, Point d) {
	b -= d , c -= d , a -= d;
	return dot_product(a, cross_product(b, d)) / 6;
}

float tetrahedron_volume(Point a, Triangle tr) {
	return tetrahedron_volume(a, tr[0], tr[1], tr[2]);
}

object::iterator object::begin() {
	return polygones.begin();
}

object::iterator object::end() {
	return polygones.end();
}

void object::init() { // calc volume and unique points
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

object::object(const std::vector<Triangle>& trianguals): polygones(trianguals) {
	init();
}

object::object(box trianguals) {
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
		for (int j = 0; j < 3; j++) {
			Point a = p;
			Point b = p;
			a[j] = trianguals.first[j];
			b[(j + 2) % 3] = trianguals.second[(j + 2) % 3];
			polygones.push_back(Triangle(a, p, b));
		}
	}
	init();
}

bool object::contains(Point p) { // calculate volume
	float temp = 0;
	for (Triangle v : polygones) {
		temp += abs(tetrahedron_volume(p, v));
	}
	return is_zero(temp - real_volume);
}

int object::cross(box limit) { // 0 - empty, 1 - obj in limit, 2 - limit in obj, 3 - limit and obj have intersection
	std::vector<Point> limit_points = limit.get_points();
	object limit_obj = limit;
	bool in = true;
	for (Point v : limit_points) {
		if (!contains(v)) {
			in = false;
		}
	}
	if (in) {
		return 1;
	}
	bool out = true;
	for (Point tpoint : points) {
		if (!limit_obj.contains(tpoint)) {
			out = false;
		}
	}
	if (out) {
		return 2;
	}
	for (Triangle v : limit_obj) {
		for (Triangle u : polygones) {
			if (cross_triangle_triangle(v, u)) {
				return 3;
			}
		}
	}
	return 0;
}

Point operator+(Point a, Point b) {
	return Point(a.x + b.x, a.y + b.y, a.z + b.z);
}

Point operator-(Point a, Point b) {
	return Point(a.x - b.x, a.y - b.y, a.z - b.z);
}

bool operator<(Point a, Point b) {
	for (int i = 0; i < 3; i++) {
		if (!is_zero(a[i] - b[i])) {
			return a[i] < b[i];
		}
	}
	return false;
}

bool operator==(Point a, Point b) {
	return is_zero(a.x - b.x) && is_zero(a.y - b.y) && is_zero(a.z - b.z);
}

bool operator<=(Point a, Point b) {
	return a < b || a == b;
}
