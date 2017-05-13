#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <vector>
#include "quadtree.h"

using namespace std;

int main() {
	freopen("input.txt", "r", stdin);
	freopen("output.txt", "w", stdout);
	vector<Object> objects;
	int n, m;
	cin >> n >> m;
	vector<Point> points(n);
	for(int i = 0; i < n; i++) {
		cin >> points[i].x >> points[i].y >> points[i].z;
	}
	vector<vector<int> > connections(m, vector<int>(3));
	for(int i = 0; i < m; i++) {
		for(int j = 0; j < 3; j++) {
			cin >> connections[i][j];
			connections[i][j]--;
		}
	}
	Object object(points, connections);
	Box limit(Point(-1, -1, -1), Point(10, 10, 10));
	objects.push_back(object);
	Quadtree quadtree(objects, limit);
	auto zone = quadtree.get_zones();
	cout << zone.size() << '\n';
	for(auto v : zone) {
		auto s = (v.second + v.first) / 2;
		auto d = v.first - v.second;
		float r = min(min(abs(d.x), abs(d.y)), abs(d.x));
		cout << s.x << ' ' << s.y << ' ' << s.z << ' ' << r << '\n';
	}
}