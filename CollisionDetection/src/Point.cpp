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
	if(this == &other)
		return;
	float x, y;
	x = other.position.x - this->position.x;
	y = other.position.y - this->position.y;

	if(x * x + y * y <= (this->size + other.size) * (this->size + other.size))
	{
		float distance = sqrtf(x * x + y * y);
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

		//float normal[2]{ (other.position.x - this->position.x) / distance, (other.position.y - this->position.y) / distance };
		//float tangent[2]{ -normal[1], normal[0] };

		//float dotProductTangent[2]{ this->speed.x * tangent[0] + this->speed.y * tangent[1],
		//	other.speed.x * tangent[0] + other.speed.y * tangent[1] };

		//float dotProductNormal[2]{ this->speed.x * normal[0] + this->speed.y * normal[1],
		//	other.speed.x * normal[0] + other.speed.y * normal[1] };
	
		//// conservation of momentum
		//float momentum[2]{ (dotProductNormal[0] * (this->size - other.size) + 2.f * other.size * dotProductNormal[1]) / (this->size + other.size),
		//	(dotProductNormal[1] * (other.size - this->size) + 2.f * this->size * dotProductNormal[0]) / (this->size + other.size) };

		//// speed = tangental response + normal response
		//this->speed.x = tangent[0] * dotProductTangent[0] + normal[0] * momentum[0];
		//this->speed.y = tangent[1] * dotProductTangent[0] + normal[1] * momentum[0];
		//other.speed.x = tangent[0] * dotProductTangent[1] + normal[0] * momentum[1];
		//other.speed.y = tangent[1] * dotProductTangent[1] + normal[1] * momentum[1];
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
