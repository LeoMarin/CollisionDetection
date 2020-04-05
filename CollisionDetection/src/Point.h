#pragma once

#include <cmath>

struct Point
{
	float Position[2];
	float Direction[2];

	Point(float posX, float posY, float dir);

	void CollisionDetection(Point& other, float pointSize);
	void Move(float speed, float boundaryX, float boundaryY);
};

struct Vertex
{
	float Position[3];

	Vertex(float x, float y, float z)
	{
		Position[0] = x;
		Position[1] = y;
		Position[2] = z;
	}
};
