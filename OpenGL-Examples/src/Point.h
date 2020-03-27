#pragma once

#include <cmath>

struct Point
{
	float Position[2];
	float Direction[2];

	Point(float posX, float posY, float dir)
	{
		Position[0] = posX;
		Position[1] = posY;
		Direction[0] = cos(dir);
		Direction[1] = sin(dir);
	}
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
