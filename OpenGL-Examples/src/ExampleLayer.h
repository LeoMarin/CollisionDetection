#pragma once

#include <GLCore.h>
#include <GLCoreUtils.h>
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
	
	GLuint m_QuadVA, m_QuadVB, m_QuadIB;

	glm::vec4 m_SquareColor = { 1.f, 1.f, 1.f, 1.0f };

	const float m_BoundaryX = 1.5f;
	const float m_BoundaryY = 1.f;
	float m_Speed = 0.005f;
	float m_PointSize = 0.005f;

	int m_NumberOfPoints = 50;
	int m_CurrentNumberOfPoints;
	const int m_MaxPoints = 10000;

	std::vector<Point> m_Points;

};