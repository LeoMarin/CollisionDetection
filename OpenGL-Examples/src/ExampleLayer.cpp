#include "ExampleLayer.h"

#include <stdlib.h>
#include <time.h> 
#include <vector>


using namespace GLCore;
using namespace GLCore::Utils;

ExampleLayer::ExampleLayer()
	: m_CameraController(16.0f / 9.0f)
{

}

ExampleLayer::~ExampleLayer()
{

}

void ExampleLayer::OnAttach()
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

void ExampleLayer::OnDetach()
{
	glDeleteVertexArrays(1, &m_QuadVA);
	glDeleteBuffers(1, &m_QuadVB);
	glDeleteBuffers(1, &m_QuadIB);
}

void ExampleLayer::OnEvent(Event& event)
{

}

void ExampleLayer::OnUpdate(Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	// Set dynamic vertex buffer	
	std::vector<Vertex> vertices;
	GenerateVerices(vertices);

	MovePoints();

	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), &vertices[0]);

	glClear(GL_COLOR_BUFFER_BIT);


	glClearColor(1.f, 1.f, 1.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_Shader->GetRendererID());

	int location = glGetUniformLocation(m_Shader->GetRendererID(), "u_ViewProjection");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m_CameraController.GetCamera().GetViewProjectionMatrix()));

	location = glGetUniformLocation(m_Shader->GetRendererID(), "u_Color");

	glBindVertexArray(m_QuadVA);
	glDrawElements(GL_TRIANGLES, m_NumberOfPoints * 6, GL_UNSIGNED_INT, nullptr);
}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Controls");
	// TODO remove hardcoded
	ImGui::SliderFloat("Point size", &m_PointSize, .001f, .02f);
	ImGui::SliderFloat("Speed", &m_Speed, 0.0001f, m_PointSize);
	ImGui::SliderInt("Number of points", &m_NumberOfPoints, 10, m_MaxPoints);
	ImGui::End();
}


void ExampleLayer::GeneratePoints()
{
	// TODO fix constant number of points
	m_Points.reserve(m_MaxPoints);

	for (int i = 0; i < m_MaxPoints; i++)
	{
		m_Points.emplace_back(
		((double)rand() / (RAND_MAX)) * 3 - 1.5,
			((double)rand() / (RAND_MAX)) * 2 - 1,
			rand() % 360
			);
	}
}

void ExampleLayer::GenerateVerices(std::vector<Vertex>& vertices)
{
	vertices.reserve(m_NumberOfPoints * 4);

	for (int i = 0; i < m_NumberOfPoints; i++)
	{
		vertices.emplace_back(m_Points[i].Position[0] - m_PointSize, m_Points[i].Position[1] + m_PointSize, 0.0f);
		vertices.emplace_back(m_Points[i].Position[0] - m_PointSize, m_Points[i].Position[1] - m_PointSize, 0.0f);
		vertices.emplace_back(m_Points[i].Position[0] + m_PointSize, m_Points[i].Position[1] - m_PointSize, 0.0f);
		vertices.emplace_back(m_Points[i].Position[0] + m_PointSize, m_Points[i].Position[1] + m_PointSize, 0.0f);
	}
}

void ExampleLayer::GenerateIndices(std::vector<uint32_t>& indices)
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


void ExampleLayer::MovePoints()
{
	for (int i = 0; i < m_NumberOfPoints; i++)
	{
		m_Points[i].Position[0] += m_Points[i].Direction[0] * m_Speed;
		m_Points[i].Position[1] += m_Points[i].Direction[1] * m_Speed;

		// Detect boundary collision
		if (m_Points[i].Position[0] > m_BoundaryX || m_Points[i].Position[0] < -m_BoundaryX)
			m_Points[i].Direction[0] = -m_Points[i].Direction[0];

		if (m_Points[i].Position[1] > m_BoundaryY || m_Points[i].Position[1] < -m_BoundaryY)
			m_Points[i].Direction[1] = -m_Points[i].Direction[1];
	}
}