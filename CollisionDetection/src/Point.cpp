#include "Point.h"

Point::Point(float posX, float posY, float speedX, float speedY, float size)
{
	this->size = size;
	position.x = posX;
	position.y = posY;
	speed.x = speedX;
	speed.y = speedY;
}

void Point::CollisionDetection(Point& other)
{
	float x, y;
	x = other.position.x - this->position.x;
	y = other.position.y - this->position.y;

	if(x * x + y * y <= (this->size + other.size) * (this->size + other.size))
	{
		float distance = sqrtf(x * x + y * y) + 0.00000001f;
		float overlap = (distance - this->size - other.size) / 2;
		
		Vector2 displacment{ overlap * (this->position.x - other.position.x) / distance,
			overlap * (this->position.y - other.position.y) / distance };
		
		this->position.x -= displacment.x;
		this->position.y -= displacment.y;
		other.position.x += displacment.x;
		other.position.y += displacment.y;

		float kx = this->speed.x - other.speed.x;
		float ky = this->speed.y - other.speed.y;

		Vector2 normal{ x / distance, y / distance };

		float p = 2.f * (normal.x * kx + normal.y * ky) / (this->size + other.size);
		this->speed.x -= p * other.size * normal.x;
		this->speed.y -= p * other.size * normal.y;
		other.speed.x += p * this->size * normal.x;
		other.speed.y += p * this->size * normal.y;
	}
}

void Point::Move(float speed, float boundaryX, float boundaryY)
{
	this->position.x += this->speed.x * speed;
	this->position.y += this->speed.y * speed;

	// Detect boundary collision
	if(this->position.x > boundaryX)
	{
		this->speed.x = -this->speed.x;
		this->position.x = boundaryX;
	}
	else if(this->position.x < -boundaryX)
	{
		this->speed.x = -this->speed.x;
		this->position.x = -boundaryX;
	}

	if(this->position.y > boundaryY)
	{
		this->speed.y = -this->speed.y;
		this->position.y = boundaryY;
	}
	else if(this->position.y < -boundaryY)
	{
		this->speed.y = -this->speed.y;
		this->position.y = -boundaryY;

	}
}
