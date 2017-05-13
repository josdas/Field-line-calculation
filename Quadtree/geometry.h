#pragma once
#include <vector>

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

	float& operator[](size_t t);
	float operator[](size_t t) const;

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
	Point operator[](size_t x);
};

struct Interval {
	float min;
	float max;
};

Interval get_interval(Triangle triangle, Point axis);
bool overlap_on_axis(Triangle t1, Triangle t2, Point axis);
bool cross_triangle_triangle(Triangle t1, Triangle t2);

struct Box {
	union {
		struct {
			Point first, second;
		};
		Point data[2];
	};

	Box(Point first, Point second);
	Box(Box const& b) {
		data[0] = b.data[0];
		data[1] = b.data[1];
	}
	bool contains(Point x) const;
	std::vector<Point> get_points() const;
};

float tetrahedron_volume(Point a, Point b, Point c, Point d);
float tetrahedron_volume(Point a, Triangle tr);

enum ÑrossType {
	LIMIT_IN_OBJ,
	OBJ_IN_LIMIT,
	INTERSECTION,
	EMPTY
};

class Object {
	std::vector<Triangle> polygones;
	std::vector<Point> points;
	float real_volume;

	void init();
public:
	typedef std::vector<Triangle>::iterator iterator;
	iterator begin();
	iterator end();

	Object(const std::vector<Point>& points, std::vector<std::vector<int> > connect);
	Object(const std::vector<Triangle>& trianguals);
	Object(Box trianguals);

	bool contains(Point p);
	ÑrossType cross(Box limit);
};

class PObject : public Object{
	float charge;
public:
	PObject(const std::vector<Point>& points, std::vector<std::vector<int> > connect, float charge);
	PObject(const std::vector<Triangle>& trianguals, float charge);
	PObject(Box trianguals, float charge);
	PObject(Object const& object, float charge);

	float get_charge() const;
};