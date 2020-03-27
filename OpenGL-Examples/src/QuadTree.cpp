#include "QuadTree.h"

QuadTree::QuadTree()
{
	isLeaf = true;
	parentNode = nullptr;

	xMax = 1.5f;
	xMin = -1.5f;
	yMax = 1.f;
	yMin = -1.f;
}

QuadTree::QuadTree(QuadTree& parent, int childNumber)
{
	isLeaf = true;
	parentNode = &parent;

	switch(childNumber)
	{
	case 0:
		xMax = parent.xMax;
		yMax = parent.yMax;
		xMin = (parent.xMax + parent.xMin) / 2;
		yMin = (parent.yMax + parent.yMin) / 2;
		break;
	case 1:
		xMax = (parent.xMax + parent.xMin) / 2;
		yMax = parent.yMax;
		xMin = parent.xMin;
		yMin = (parent.yMax + parent.yMin) / 2;
		break;
	case 2:
		xMax = (parent.xMax + parent.xMin) / 2;
		yMax = (parent.yMax + parent.yMin) / 2;
		xMin = parent.xMin;
		yMin = parent.yMin;
		break;
	case 3:
		xMax = parent.xMax;
		yMax = (parent.yMax + parent.yMin) / 2;
		xMin = (parent.xMax + parent.xMin) / 2;
		yMin = parent.xMin;
		break;
	default:
		break;
	}

}

void QuadTree::CreateChildNodes()
{
	for(int i = 0; i < 4; i++)
	{
		childNodes[i] = new QuadTree(*this, i);
	}

	while(!points.empty())
	{
		for(int i = 0; i < 4; i++)
		{
			if(childNodes[i]->PointIsInsideQuad(*points.back()))
			{
				childNodes[i]->AddPoint(*points.back());
				points.pop_back();
				break;
			}
		}
	}
}

void QuadTree::AddPoint(Point& point)
{
	if(!isLeaf)
	{

	}
	else
	{
		points.push_back(&point);
		if(points.size() > 4)
		{
			this->CreateChildNodes();
		}
	}
}

bool QuadTree::PointIsInsideQuad(const Point& point)
{
	if(point.Position[0] > this->xMax ||
		point.Position[0] < this->xMin ||
		point.Position[1] > this->yMax ||
		point.Position[1] < this->yMin)
	{
		return false;
	}
	else
	{
		return true;
	}
}
