#pragma once

#include <cmath>

struct Vector2
{
	float x;
	float y;
};

struct Point
{
	Vector2 position;
	Vector2 speed;
	float size;

	Point(float posX, float posY, float speedX, float speedY, float size);

	void CollisionDetection(Point& other);
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
