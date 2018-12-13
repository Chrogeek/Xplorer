#ifndef XPLORER_GEOMETRY_H
#define XPLORER_GEOMETRY_H

#include <vector>

struct pointVector {
	double vX, vY;
	pointVector(double = 0.f, double = 0.f);
	pointVector operator+(pointVector) const;
	pointVector operator-(pointVector) const;
	pointVector operator-() const;
	pointVector operator*(double) const;
	pointVector operator/(double) const;
	pointVector &operator+=(pointVector);
	pointVector &operator-=(pointVector);
	double length() const;
	double polar() const;
};

pointVector operator*(double, pointVector);

double dot(pointVector, pointVector);
double cross(pointVector, pointVector);
double angle(pointVector, pointVector);
double distance(pointVector, pointVector);

struct polygon {
	std::vector<pointVector> vertex; // list of vertices
	double area();
	int count(); // # of vertices
	int cover(pointVector); // returns whether the polygon covers the point specified or not
	void counterclockwise(); // make the vertex list counterclockwise
	polygon();
};

struct line {
	pointVector p, v;
	line(pointVector = pointVector(0.0, 0.0), pointVector = pointVector(1.0, 0.0));
	double polar();
};

bool isPointOnSegment(pointVector, pointVector, pointVector); // whether the point is on the segment. returns -1 if the point is on the boundary.

bool isOnLeft(line, pointVector);
pointVector lineIntersection(line, line);
polygon semiplaneIntersection(std::vector<line> &);
bool isPolygonIntersect(polygon, polygon);

#endif
