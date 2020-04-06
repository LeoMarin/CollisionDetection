#include "CollisionDetection.h"

#include <stdlib.h>
#include <time.h> 
#include <vector>
#include <cmath>

using namespace GLCore;
using namespace GLCore::Utils;

CollisionDetection::CollisionDetection()
	: m_CameraController(16.0f / 9.0f)
{

}

CollisionDetection::~CollisionDetection()
{

}

void CollisionDetection::OnAttach()
{
	EnableGLDebugging();
	srand(time(NULL));

	GeneratePoints();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_Shader = Shader::FromGLSLTextFiles(
		"assets/shaders/test.vert.glsl",
		"assets/shaders/test.frag.glsl"
	);
	
	PointRenderingSetup();
	AccelerationRenderingSetup(); // TODO fix number of indices

	quadTree.GenerateQuadTree(m_Points, m_NumberOfPoints);
}

void CollisionDetection::OnDetach()
{
	glDeleteVertexArrays(1, &m_QuadVA);
	glDeleteBuffers(1, &m_QuadVB);
	glDeleteBuffers(1, &m_QuadIB);


	glDeleteVertexArrays(1, &m_AccelerationVA);
	glDeleteBuffers(1, &m_AccelerationVB);
	glDeleteBuffers(1, &m_AccelerationIB);
}

void CollisionDetection::OnEvent(Event& event)
{

}

void CollisionDetection::OnUpdate(Timestep ts)
{
	m_CameraController.OnUpdate(ts);


	glUseProgram(m_Shader->GetRendererID());

	int location = glGetUniformLocation(m_Shader->GetRendererID(), "u_ViewProjection");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m_CameraController.GetCamera().GetViewProjectionMatrix()));

	location = glGetUniformLocation(m_Shader->GetRendererID(), "u_Color");

	glClear(GL_COLOR_BUFFER_BIT);

	glClearColor(1.f, 1.f, 1.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	MovePoints();
	DrawPoints();

	switch(collisionSystem)
	{
	case 0:
		BruteForceCollisionDetection();
		break;
	case 1:
		QuadTreeCollisionDetection();
		break;
	case 2:
		SpatialHashingCollisionDetection();
		break;
	default:
		break;
	}

}

void CollisionDetection::OnImGuiRender()
{
	ImGui::Begin("Controls");
	// TODO remove hardcoded
	ImGui::SliderFloat("Speed", &m_Speed, 0.0001f, m_PointSize);
	ImGui::SliderInt("Number of points", &m_NumberOfPoints, 10, m_MaxPoints);
	ImGui::SliderInt("BF - 0, QT - 1", &collisionSystem, 0, 2);
	ImGui::End();
}


void CollisionDetection::GeneratePoints()
{
	// TODO fix constant number of points
	m_Points.reserve(m_MaxPoints);

	for (int i = 0; i < m_MaxPoints; i++)
	{
		m_Points.emplace_back(
		((double)rand() / (RAND_MAX)) * 2 - 0.5f,
			((double)rand() / (RAND_MAX)) * 1.5 -0.5f,
			((double)rand() / (RAND_MAX)) * 0.5 - 0.25f,
			((double)rand() / (RAND_MAX)) * 0.5 - 0.25f,
			m_PointSize
			);
	}
}

void CollisionDetection::GenerateVerices(std::vector<Vertex>& vertices)
{
	vertices.reserve(m_NumberOfPoints * 4);

	for (int i = 0; i < m_NumberOfPoints; i++)
	{
		vertices.emplace_back(m_Points[i].position.x - m_PointSize, m_Points[i].position.y + m_PointSize, 0.0f);
		vertices.emplace_back(m_Points[i].position.x - m_PointSize, m_Points[i].position.y - m_PointSize, 0.0f);
		vertices.emplace_back(m_Points[i].position.x + m_PointSize, m_Points[i].position.y - m_PointSize, 0.0f);
		vertices.emplace_back(m_Points[i].position.x + m_PointSize, m_Points[i].position.y + m_PointSize, 0.0f);
	}
}

void CollisionDetection::GenerateIndices(std::vector<uint32_t>& indices)
{
	indices.reserve(m_MaxPoints * 6);
	for (int i = 0; i < m_MaxPoints; i++)
	{
		indices.emplace_back(i * 4);
		indices.emplace_back(i * 4 + 1);
		indices.emplace_back(i * 4 + 2);
		indices.emplace_back(i * 4 + 2);
		indices.emplace_back(i * 4 + 3);
		indices.emplace_back(i * 4);
	}
}

void CollisionDetection::PointRenderingSetup()
{
	glCreateVertexArrays(1, &m_QuadVA);
	glBindVertexArray(m_QuadVA);

	glCreateBuffers(1, &m_QuadVB);
	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_MaxPoints * 4, nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Position));

	std::vector<uint32_t> indices;
	GenerateIndices(indices);

	glCreateBuffers(1, &m_QuadIB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_QuadIB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), &indices[0], GL_STATIC_DRAW);
}

void CollisionDetection::AccelerationRenderingSetup()
{
	glCreateVertexArrays(1, &m_AccelerationVA);
	glBindVertexArray(m_AccelerationVA);

	glCreateBuffers(1, &m_AccelerationVB);
	glBindBuffer(GL_ARRAY_BUFFER, m_AccelerationVB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_MaxPoints, nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Position));

	std::vector<uint32_t> treeIndices;

	treeIndices.reserve(m_MaxPoints);
	for(int i = 0; i < m_MaxPoints; i++)
	{
		treeIndices.emplace_back(i);
	}

	glCreateBuffers(1, &m_AccelerationIB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_AccelerationIB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, treeIndices.size() * sizeof(float), &treeIndices[0], GL_STATIC_DRAW);
}

void CollisionDetection::DrawPoints()
{
	std::vector<Vertex> vertices;
	GenerateVerices(vertices);

	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), &vertices[0]);

	glBindVertexArray(m_QuadVA);
	glDrawElements(GL_TRIANGLES, m_NumberOfPoints * 6, GL_UNSIGNED_INT, nullptr);
}

void CollisionDetection::DrawQuadTree()
{
	std::vector<Vertex> treeVertices;


	treeVertices.emplace_back(-1.5f, 1.f, 0.f);
	treeVertices.emplace_back(-1.5f, -1.f, 0.f);
	treeVertices.emplace_back(-1.5f, .999f, 0.f);
	treeVertices.emplace_back(1.5f, .999f, 0.f);
	treeVertices.emplace_back(-1.5f, -1.f, 0.f);
	treeVertices.emplace_back(1.5f, -1.f, 0.f);
	treeVertices.emplace_back( 1.5f, -1.f, 0.f);
	treeVertices.emplace_back( 1.5f,  1.f, 0.f);

	quadTree.CreateQuadTreeVertices(treeVertices);
	
	int numberOfVertices = treeVertices.size();
	glBindBuffer(GL_ARRAY_BUFFER, m_AccelerationVB);
	glBufferSubData(GL_ARRAY_BUFFER, 0, treeVertices.size() * sizeof(Vertex), &treeVertices[0]);

	glBindVertexArray(m_AccelerationVA);
	glLineWidth(1.f);
	glDrawElements(GL_LINES, numberOfVertices, GL_UNSIGNED_INT, nullptr);

}

void CollisionDetection::DrawSpatialHash(SpatialHash& spatialHash)
{
	std::vector<Vertex> spatialHashVertices;

	spatialHashVertices.emplace_back(-1.5f, 1.f, 0.f);
	spatialHashVertices.emplace_back(-1.5f, -1.f, 0.f);
	spatialHashVertices.emplace_back(-1.5f, .999f, 0.f);
	spatialHashVertices.emplace_back(1.5f, .999f, 0.f);
	spatialHashVertices.emplace_back(-1.5f, -1.f, 0.f);
	spatialHashVertices.emplace_back(1.5f, -1.f, 0.f);
	spatialHashVertices.emplace_back(1.5f, -1.f, 0.f);
	spatialHashVertices.emplace_back(1.5f, 1.f, 0.f);

	spatialHash.GenerateSpatialHashVertices(spatialHashVertices);

	int numberOfVertices = spatialHashVertices.size();
	glBindBuffer(GL_ARRAY_BUFFER, m_AccelerationVB);
	glBufferSubData(GL_ARRAY_BUFFER, 0, spatialHashVertices.size() * sizeof(Vertex), &spatialHashVertices[0]);

	glBindVertexArray(m_AccelerationVA);
	glLineWidth(1.f);

	glDrawElements(GL_LINES, numberOfVertices, GL_UNSIGNED_INT, nullptr);
}

void CollisionDetection::MovePoints()
{
	for (int i = 0; i < m_NumberOfPoints; i++)
	{
		m_Points[i].Move(m_Speed, m_BoundaryX, m_BoundaryY);
	}
}

void CollisionDetection::BruteForceCollisionDetection()
{
	for (int i = 0; i < m_NumberOfPoints - 1; i++)
	{
		for (int j = i+1; j < m_NumberOfPoints; j++)
		{
			m_Points[i].CollisionDetection(m_Points[j]);
		}
	}
}

void CollisionDetection::QuadTreeCollisionDetection()
{
	if(m_PreviousNumberOfPoints != m_NumberOfPoints)
	{
		quadTree.~QuadTree();
		quadTree.GenerateQuadTree(m_Points, m_NumberOfPoints);
		m_PreviousNumberOfPoints = m_NumberOfPoints;
	}
	else
	{
		quadTree.Redristribute();
		quadTree.DeleteChildNodes();
	}

	quadTree.CollisionDetection();
	DrawQuadTree();
}

void CollisionDetection::SpatialHashingCollisionDetection()
{
	SpatialHash spatialHash{ m_BoundaryX, m_BoundaryY, 100, 100 };
	spatialHash.FillSpatialHashTable(m_Points, m_NumberOfPoints);
	spatialHash.CollisionDetection();
	DrawSpatialHash(spatialHash);
}