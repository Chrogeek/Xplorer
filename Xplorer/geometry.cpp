#include "defs.h"
#include <cmath>
#include <algorithm>
#include "geometry.h"

pointVector::pointVector(double x, double y) : vX(x), vY(y) {}

pointVector pointVector::operator+(pointVector b) const {
	return pointVector(vX + b.vX, vY + b.vY);
}

pointVector pointVector::operator-(pointVector b) const {
	return pointVector(vX - b.vX, vY - b.vY);
}

pointVector pointVector::operator-() const {
	return pointVector(-vX, -vY);
}

pointVector pointVector::operator*(double b) const {
	return pointVector(vX * b, vY * b);
}

pointVector pointVector::operator/(double b) const {
	return pointVector(vX / b, vY / b);
}

pointVector &pointVector::operator+=(pointVector b) {
	vX += b.vX, vY += b.vY;
	return *this;
}

pointVector &pointVector::operator-=(pointVector b) {
	vX -= b.vX, vY -= b.vY;
	return *this;
}

double pointVector::length() const {
	return sqrt(vX * vX + vY * vY);
}

double pointVector::polar() const {
	return atan2(vY, vX);
}

pointVector operator*(double a, pointVector b) {
	return pointVector(a * b.vX, a * b.vY);
}

double dot(pointVector a, pointVector b) {
	return a.vX * b.vX + a.vY * b.vY;
}

double cross(pointVector a, pointVector b) {
	return a.vX * b.vY - a.vY * b.vX;
}

double angle(pointVector a, pointVector b) {
	return acos(dot(a, b) / a.length() / b.length());
}

double distance(pointVector a, pointVector b) {
	return (a - b).length();
}

double polygon::area() {
	double ans = 0.0;
	for (int i = count() - 1; i >= 2; --i)
		ans += cross(vertex[i - 1] - vertex[0], vertex[i] - vertex[0]);
	return ans / 2.0;
}

int polygon::count() {
	return (int)vertex.size();
}

int polygon::cover(pointVector p) {
	int wn = 0, n = count(); // winding number
	for (int i = 0; i < n; ++i) {
		pointVector &p1 = vertex[i], &p2 = vertex[(i + 1) % n];
		if (isPointOnSegment(p, p1, p2)) return -1;
		int k = dcmp(cross(p2 - p1, p - p1));
		int d1 = dcmp(p1.vY, p.vY);
		int d2 = dcmp(p2.vY, p.vY);
		if (k > 0 && d1 <= 0 && d2 > 0) ++wn; // counterclockwise
		if (k < 0 && d2 <= 0 && d1 > 0) --wn; // clockwise
	}
	return wn ? 1 : 0;
}

void polygon::counterclockwise() {
	if (dcmp(area()) < 0) std::reverse(vertex.begin(), vertex.end());
}

polygon::polygon() { vertex.clear(); }

line::line(pointVector p, pointVector v) {
	this->p = p, this->v = v;
}

double line::polar() {
	return v.polar();
}

bool isPointOnSegment(pointVector p, pointVector p1, pointVector p2) { // whether the point p is on segment p1-p2
	return dcmp(cross(p1 - p, p2 - p)) == 0 && dcmp(dot(p1 - p, p2 - p)) < 0;
}

bool isOnLeft(line l, pointVector p) {
	return cross(l.v, p - l.p) > 0.0;
}

pointVector lineIntersection(line a, line b) {
	// returns the intersection point of the two lines
	// answer assumed exist
	pointVector v = a.p - b.p;
	double t = cross(b.v, v) / cross(a.v, b.v);
	return a.p + a.v * t;
}

polygon semiplaneIntersection(std::vector<line> &lines) {
	std::sort(lines.begin(), lines.end(), [](line a, line b) { return a.polar() < b.polar(); });
	int n = (int)lines.size(), head, tail;
	pointVector *p = new pointVector[n];
	line *q = new line[n];
	q[head = tail = 0] = lines[0];
	for (int i = 1; i < n; ++i) {
		while (head < tail && !isOnLeft(lines[i], p[tail - 1])) --tail;
		while (head < tail && !isOnLeft(lines[i], p[head])) ++head;
		q[++tail] = lines[i];
		if (dcmp(cross(q[tail].v, q[tail - 1].v)) == 0) {
			--tail;
			if (isOnLeft(q[tail], lines[i].p)) q[tail] = lines[i];
		}
		if (head < tail) p[tail - 1] = lineIntersection(q[tail - 1], q[tail]);
	}
	while (head < tail && !isOnLeft(q[head], p[tail - 1])) --tail;

	polygon ans;
	if (tail - head > 1) {
		p[tail] = lineIntersection(q[tail], q[head]);
		for (int i = head; i <= tail; ++i) ans.vertex.push_back(p[i]);
	}
	delete[] p;
	delete[] q;
	return ans;
}

bool isPolygonIntersect(polygon a, polygon b) {
	a.counterclockwise();
	b.counterclockwise();
	std::vector<line> lines;
	lines.clear();
	for (int i = 0, n = a.count(); i < n; ++i) lines.push_back(line(a.vertex[i], a.vertex[(i + 1) % n] - a.vertex[i]));
	for (int i = 0, m = b.count(); i < m; ++i) lines.push_back(line(b.vertex[i], b.vertex[(i + 1) % m] - b.vertex[i]));
	return semiplaneIntersection(lines).count() > 0;
}
