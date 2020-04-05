#include "Point.h"

Point::Point(float posX, float posY, float dir)
{
	Position[0] = posX;
	Position[1] = posY;
	Direction[0] = cos(dir);
	Direction[1] = sin(dir);
}

void Point::CollisionDetection(Point& other, float pointSize)
{
	float x, y;
	x = abs(this->Position[0] - other.Position[0]);
	y = abs(this->Position[1] - other.Position[1]);

	if(x < pointSize * 2 && y < pointSize * 2)
	{
		if(x > y)
		{
			this->Direction[0] = -this->Direction[0];
			other.Direction[0] = -other.Direction[0];
		}
		else
		{
			this->Direction[1] = -this->Direction[1];
			other.Direction[1] = -other.Direction[1];
		}
	}
}

void Point::Move(float speed, float boundaryX, float boundaryY)
{
	this->Position[0] += this->Direction[0] * speed;
	this->Position[1] += this->Direction[1] * speed;

	// Detect boundary collision
	if(this->Position[0] > boundaryX)
	{
		this->Direction[0] = -this->Direction[0];
		this->Position[0] = boundaryX;
	}
	else if(this->Position[0] < -boundaryX)
	{
		this->Direction[0] = -this->Direction[0];
		this->Position[0] = -boundaryX;
	}

	if(this->Position[1] > boundaryY)
	{
		this->Direction[1] = -this->Direction[1];
		this->Position[1] = boundaryY;
	}
	else if(this->Position[1] < -boundaryY)
	{
		this->Direction[1] = -this->Direction[1];
		this->Position[1] = -boundaryY;

	}
}
