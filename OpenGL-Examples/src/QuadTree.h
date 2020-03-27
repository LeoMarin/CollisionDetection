#pragma once

#include "Point.h"
#include <vector>

class QuadTree
{
public:
	QuadTree(); // creating parent node
	QuadTree(QuadTree& parent, int childNumber); // creating child node

	void CreateChildNodes();
	void AddPoint(Point& point);
	bool PointIsInsideQuad(const Point& point);

private:
	bool isLeaf;
	QuadTree* parentNode;
	QuadTree* childNodes[4] = { nullptr }; // order: up-right, up-left, down-left, down-right
	std::vector<Point*> points;

	float xMax, xMin, yMax, yMin;

};