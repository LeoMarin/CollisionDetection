#include "SpatialHash.h"

SpatialHash::SpatialHash(float boundaryX, float boundaryY, int numberOfCellsPerRow, int numberOfCellsPerColumn)

{
	m_HashedPoints.reserve(numberOfCellsPerRow * numberOfCellsPerColumn);
	m_BoundaryX = boundaryX;
	m_BoundaryY = boundaryY;
	m_NumberofCellsPerRow = numberOfCellsPerRow;
	m_NumberOfCellsPerColumn = numberOfCellsPerColumn;
	for(int i = 0; i < numberOfCellsPerRow * numberOfCellsPerColumn; i++)
	{
		m_HashedPoints.emplace_back(NULL);
	}
}

SpatialHash::~SpatialHash()
{
	
}

int SpatialHash::GetCellIndex(Point& point)
{
	int horizontalIndex = ((point.position.x + m_BoundaryX) / (m_BoundaryX * 2.0001f)) * m_NumberofCellsPerRow;
	int verticalIndex = ((-point.position.y + m_BoundaryY) / (m_BoundaryY * 2.0001f)) * m_NumberOfCellsPerColumn;

	return horizontalIndex + verticalIndex * m_NumberofCellsPerRow;
}

void SpatialHash::FillSpatialHashTable(std::vector<Point>& points, int numberOfPoints)
{
	for(int i = 0; i < numberOfPoints; i++)
	{
		AddPoint(&points[i]);
	}
}

void SpatialHash::Redistribute()
{
	for(int i = 0; i < m_HashedPoints.size(); i++)
	{
		int numberOfPoints = m_HashedPoints[i].size();
		for(int j = 0; j < numberOfPoints; j++)
		{
			if(i != GetCellIndex(*m_HashedPoints[i][j]))
			{
				AddPoint(m_HashedPoints[i][j]);
				m_HashedPoints[i][j] = m_HashedPoints[i][numberOfPoints - 1];
				m_HashedPoints[i].pop_back();
				numberOfPoints--;
			}
		}
	}
}

void SpatialHash::AddPoint(Point* p)
{
	m_HashedPoints[GetCellIndex(*p)].push_back(p);
}

void SpatialHash::CollisionDetection()
{
	for(int i = 0; i < m_HashedPoints.size(); i++)
	{
		int numberOfPoints = m_HashedPoints[i].size();
		for(int j = 0; j < numberOfPoints; j++)
		{

			for(int x = j + 1; x < numberOfPoints; x++)
			{
				m_HashedPoints[i][j]->CollisionDetection(*m_HashedPoints[i][x]);
			}
			CheckAdjecentCells(*m_HashedPoints[i][j], i);
		}
	}
}

void SpatialHash::CheckAdjecentCells(Point& point, int i)
{
	if(i % m_NumberofCellsPerRow != 0)
	{
		// check cell to the left
		for(Point* p : m_HashedPoints[i - 1])
			point.CollisionDetection(*p);

	}

	if((i + 1) % m_NumberofCellsPerRow != 0)
	{
		// check cell to the right
		for(Point* p : m_HashedPoints[i + 1])
			point.CollisionDetection(*p);
	}

	if(i >= m_NumberofCellsPerRow)
	{
		// check cell above
		for(Point* p : m_HashedPoints[i - m_NumberofCellsPerRow])
			point.CollisionDetection(*p);
	}

	if(i + m_NumberofCellsPerRow < m_HashedPoints.size())
	{
		// check cell below
		for(Point* p : m_HashedPoints[i + m_NumberofCellsPerRow])
			point.CollisionDetection(*p);
	}
}

void SpatialHash::GenerateSpatialHashVertices(std::vector<Vertex>& vertices)
{
	float horizontalDistance = (m_BoundaryX * 2) / m_NumberofCellsPerRow;
	float verticalDistance = (m_BoundaryY * 2) / m_NumberOfCellsPerColumn;

	for(int i = 1; i < m_NumberofCellsPerRow; i++)
	{
		vertices.emplace_back(i * horizontalDistance - m_BoundaryX, m_BoundaryY, 0.f);
		vertices.emplace_back(i * horizontalDistance - m_BoundaryX, -m_BoundaryY, 0.f);
	}

	for(int i = 1; i < m_NumberOfCellsPerColumn; i++)
	{
		vertices.emplace_back(m_BoundaryX, i*verticalDistance-m_BoundaryY, 0.f);
		vertices.emplace_back(-m_BoundaryX, i*verticalDistance-m_BoundaryY, 0.f);
	}
}

void SpatialHash::ClearPoints()
{
	for(std::vector<Point*>& cell : m_HashedPoints)
	{
		cell.clear();
		cell.shrink_to_fit();
	}
}
