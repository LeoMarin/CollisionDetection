#pragma once

#include "Point.h"
#include "QuadTree.h"

#include <GLCore.h>
#include <GLCoreUtils.h>

class ExampleLayer : public GLCore::Layer
{
public:

	ExampleLayer();
	virtual ~ExampleLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(GLCore::Event& event) override;
	virtual void OnUpdate(GLCore::Timestep ts) override;
	virtual void OnImGuiRender() override;

	void PointRenderingSetup();
	void QuadTreeRenderingSetup();
	void DrawPoints();
	void DrawQuadTree();

	void MovePoints();
	void GeneratePoints();
	void GenerateVerices(std::vector<Vertex>& vertices);
	void GenerateIndices(std::vector<uint32_t>& indices);

	void BruteForceCollisionDetection();
	void QuadTreeCollisionDetection();
	void SpatialHashingCollisionDetection();

private:
	GLCore::Utils::Shader* m_Shader;
	GLCore::Utils::OrthographicCameraController m_CameraController;
	
	GLuint m_QuadVA, m_QuadVB, m_QuadIB, m_TreeVA, m_TreeVB, m_TreeIB;

	glm::vec4 m_SquareColor = { 1.f, 1.f, 1.f, 1.0f };

	const float m_BoundaryX = 1.5f;
	const float m_BoundaryY = 1.f;
	float m_Speed = 0.005f;
	float m_PointSize = 0.005f;

	int m_NumberOfPoints = 5000;
	int m_PreviousNumberOfPoints = m_NumberOfPoints;
	const int m_MaxPoints = 10000;

	int collisionSystem = 0;

	QuadTree quadTree{};
	std::vector<Point> m_Points;

};