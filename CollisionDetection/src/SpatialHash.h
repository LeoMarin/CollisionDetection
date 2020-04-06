#pragma once

#include <vector>
#include <cmath>
#include "Point.h"
#include <iostream>

class SpatialHash
{
public:
	SpatialHash(float boundaryX, float boundaryY, int numberOfCellsPerRow, int numberOfCellsPerColumn);
	~SpatialHash();

	// Hash function
	int GetCellIndex(Point& point);

	void FillSpatialHashTable(std::vector<Point>& points, int numberOfPoints);

	void Redistribute();

	void AddPoint(Point* p);

	void CollisionDetection();

	void GenerateSpatialHashVertices(std::vector<Vertex>& vertices);

private:
	float m_BoundaryX, m_BoundaryY;
	int m_NumberOfCellsPerColumn, m_NumberofCellsPerRow;
	std::vector<std::vector<Point*>> m_HashedPoints;
};