#include "QuadTree.h"
#include <iostream>

QuadTree::QuadTree()
{
	parentNode = nullptr;
	xMax = 1.5f;
	xMin = -1.5f;
	yMax = 1.f;
	yMin = -1.f;
}

QuadTree::QuadTree(QuadTree& parent, ChildPosition childPosition)
{
	parentNode = &parent;
	switch(childPosition)
	{
	case ChildPosition::UpRight:
		xMax = parent.xMax;
		yMax = parent.yMax;
		xMin = (parent.xMax + parent.xMin) / 2;
		yMin = (parent.yMax + parent.yMin) / 2;
		break;
	case ChildPosition::UpLeft:
		xMax = (parent.xMax + parent.xMin) / 2;
		yMax = parent.yMax;
		xMin = parent.xMin;
		yMin = (parent.yMax + parent.yMin) / 2;
		break;
	case ChildPosition::DownLeft:
		xMax = (parent.xMax + parent.xMin) / 2;
		yMax = (parent.yMax + parent.yMin) / 2;
		xMin = parent.xMin;
		yMin = parent.yMin;
		break;
	case ChildPosition::DownRight:
		xMax = parent.xMax;
		yMax = (parent.yMax + parent.yMin) / 2;
		xMin = (parent.xMax + parent.xMin) / 2;
		yMin = parent.yMin;
		break;
	default:
		break;
	}

}

QuadTree::~QuadTree()
{
	for(int i = 0; i < 4; i++)
	{	
		if(childNodes[i] != nullptr)
		{
			delete childNodes[i];
			childNodes[i] = nullptr;
		}
	}
}

void QuadTree::GenerateQuadTree(std::vector<Point>& points, int numberOfPoints)
{
	for(int i = 0; i < numberOfPoints; i++)
	{
		AddPoint(points[i]);
	}
}

void QuadTree::CreateChildNodes()
{
	childNodes[0] = new QuadTree(*this, ChildPosition::UpRight);
	childNodes[1] = new QuadTree(*this, ChildPosition::UpLeft);
	childNodes[2] = new QuadTree(*this, ChildPosition::DownLeft);
	childNodes[3] = new QuadTree(*this, ChildPosition::DownRight);

	for(Point* p : points)
		AddPoint(*p);

	points.clear();
	points.shrink_to_fit();
}

void QuadTree::AddPoint(Point& point)
{
	if(!PointIsInsideQuad(point))
	{
		parentNode->AddPoint(point);
		return;
	}
	if(childNodes[0] != nullptr)
	{
		for(int i = 0; i < 4; i++)
		{
			if(childNodes[i]->PointIsInsideQuad(point))
			{
				childNodes[i]->AddPoint(point);
				break;
			}
		}
	}
	else
	{
		points.push_back(&point);
		if(points.size() > maxPoints)
		{
			this->CreateChildNodes();
		}
	}
}

bool QuadTree::PointIsInsideQuad(const Point& point)
{
	if(point.position.x > this->xMax ||
		point.position.x < this->xMin ||
		point.position.y > this->yMax ||
		point.position.y < this->yMin)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void QuadTree::CreateQuadTreeVertices(std::vector<Vertex>& vertices)
{
	if(childNodes[0] != nullptr)
	{
		vertices.emplace_back(xMin, childNodes[0]->yMin, 0.f);
		vertices.emplace_back(xMax, childNodes[0]->yMin, 0.f);
		vertices.emplace_back(childNodes[0]->xMin, yMin, 0.f);
		vertices.emplace_back(childNodes[0]->xMin, yMax, 0.f);

		for(QuadTree* child : childNodes)
		{
			child->CreateQuadTreeVertices(vertices);
		}
	}
}

void QuadTree::Redistribute()
{
	if(childNodes[0] != nullptr)
	{
		for(QuadTree* child : childNodes)
		{
			child->Redistribute();
		}
	}
	else
	{

		for(int i = 0; i < points.size(); i++)
		{
			if(!PointIsInsideQuad(*points[i]))
			{
				Point* p = points[i];
				points.erase(points.begin() + i);
				parentNode->AddPoint(*p);
				i--;
			}
		}
	}
}

void QuadTree::DeleteChildNodes()
{
	if(childNodes[0] == nullptr)
		return;
	bool flag = false;

	for(int i = 0; i < 4; i++)
	{
		if(childNodes[i]->childNodes[0] != nullptr)
		{
			childNodes[i]->DeleteChildNodes();
			flag = true;
		}
	}
	if(flag) return;

	int numberOfPoints = childNodes[0]->points.size() +
		childNodes[1]->points.size() +
		childNodes[2]->points.size() +
		childNodes[3]->points.size();

	if(numberOfPoints <= maxPoints)
	{
		for(int i = 0; i < 4; i++)
		{ 
			for(int j = 0; j < childNodes[i]->points.size(); j++)
			{
				points.emplace_back(childNodes[i]->points[j]);
			}
			delete childNodes[i];
			childNodes[i] = nullptr;
		}
	}
}

void QuadTree::CollisionDetection()
{
	if(childNodes[0] == nullptr)
	{
		// detect colision inside quad
		for(int i = 0; i < points.size(); i++)
		{
			for(int j = i + 1; j < points.size(); j++)
			{
				points[i]->CollisionDetection(*points[j]);
			}
		}
	}
	else
	{
		childNodes[0]->CollisionDetection();
		childNodes[1]->CollisionDetection();
		childNodes[2]->CollisionDetection();
		childNodes[3]->CollisionDetection();
	}
}
