#pragma once

#include "Point.h"
#include <vector>

class QuadTree
{
public:

	enum class ChildPosition
	{
		UpRight = 0, UpLeft, DownLeft, DownRight, Root
	};

	QuadTree(); // creating parent node
	QuadTree(QuadTree& parent, ChildPosition childPosition); // creating child node

	~QuadTree();

	void GenerateQuadTree(std::vector<Point>& points, int numberOfPoints);

	void CreateChildNodes();
	void AddPoint(Point& point);
	bool PointIsInsideQuad(const Point& point);
	void CreateQuadTreeVertices(std::vector<Vertex>& vertices);

	void Redristribute();
	void DeleteChildNodes();

	void CollisionDetection();

	static const int maxPoints = 4;

private:
	QuadTree* parentNode;
	QuadTree* childNodes[4] = { nullptr }; // order: up-right, up-left, down-left, down-right
	std::vector<Point*> points;

	float xMax, xMin, yMax, yMin;

};