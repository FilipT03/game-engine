#include "pch.h"
#include "Renderer2D.h"
#include "Renderer/BufferLayout.h"
#include "Core/Time.h"
#include "Core/Application.h"
#include "Event/WindowEvent.h"
#include <glm/gtc/matrix_transform.hpp>

#ifdef FT_OPENGL_RENDERER
#include <glad/glad.h>

#define FT_GLSL_INCLUDE
#include "Platform/OpenGL/Shaders/Basic.vert"
#include "Platform/OpenGL/Shaders/Basic.frag"
#endif

namespace ft {
	Renderer2D::Renderer2D() {};
	Renderer2D::~Renderer2D() {}

	void Renderer2D::Init()
	{
		#ifdef FT_OPENGL_RENDERER

		auto props = Application::Get().GetWindow().GetWindowProps();
		CalculateProjectionMatrix(props.width, props.height);
		
		//m_Shapes.emplace_back(std::make_unique<Polygon>(glm::vec2{-0.4,0}, glm::vec2{ +0.4,0 }));
		m_Shapes.emplace_back(std::make_unique<Polygon>(5));
		Shape& triangle = *m_Shapes[0];

		auto basicLayout = BufferLayout({
			{ LayoutElementType::Float2, "inPosition" },
		});

		VertexArray* vao = VertexArray::Create(basicLayout);
		m_VertexBuffer = std::shared_ptr<VertexBuffer>(VertexBuffer::Create(100000));
		uint32_t size = sizeof(float) * triangle.worldVertices.size();
		triangle.vertexOffset = m_LastVertexOffset;
		m_LastVertexOffset += size;		
		m_VertexBuffer->SetData(triangle.vertexOffset, size, triangle.worldVertices.data());
		vao->SetVertexBuffer(m_VertexBuffer);

		m_IndexBuffer = std::shared_ptr<IndexBuffer>(IndexBuffer::Create(100000));
		size = sizeof(uint32_t) * triangle.indices.size();
		triangle.indexOffset= m_LastIndexOffset;
		m_LastIndexOffset += size;
		m_IndexBuffer->SetData(triangle.indexOffset, size, triangle.indices.data());
		vao->SetIndexBuffer(m_IndexBuffer);

		m_VertexArrays.push_back(std::unique_ptr<VertexArray>(vao));

		m_Shader = std::unique_ptr<Shader>(Shader::Create(basicVert, basicFrag));
		m_Shader->Bind();

		#endif
	}

	void Renderer2D::Shutdown()
	{
		#ifdef FT_OPENGL_RENDERER
		#endif
	}

	void Renderer2D::Clear()
	{
		#ifdef FT_OPENGL_RENDERER
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		#endif
	}

	void Renderer2D::SetClearColor(float r, float g, float b, float a)
	{
		#ifdef FT_OPENGL_RENDERER
		glClearColor(r, g, b, a);
		#endif
	}

	void Renderer2D::OnUpdate()
	{
		#ifdef FT_OPENGL_RENDERER

		m_Shader->SetUniformMatrix4fv("uProjection", m_Projection);
		m_VertexArrays[0]->Bind();
		auto& triangle = m_Shapes[0];
		triangle->transform.rotation += 2;
		triangle->UpdateWorldVertices();
		triangle->color.r = sin(Time::TotalTime());
		triangle->color.g = cos(Time::TotalTime());
		triangle->color.b = cos(Time::TotalTime()+glm::pi<float>()/4.0f);
		triangle->transform.scale.x = sin(Time::TotalTime())/2.0+1;
		triangle->transform.scale.y = sin(Time::TotalTime())/2.0+1;
		m_Shader->SetUniform3f("uColor", triangle->color);
		m_VertexBuffer->SetData(triangle->vertexOffset, sizeof(float) * triangle->worldVertices.size(), triangle->worldVertices.data());
		glDrawElementsBaseVertex(GL_TRIANGLES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, (void*)triangle->indexOffset, triangle->vertexOffset);

		#endif
	}

	void Renderer2D::OnEvent(const Event& event)
	{
		if (event.Type == EventType::WindowResize)
		{
			auto& resizeEvent = As<WindowResizeEvent>(event);
			CalculateProjectionMatrix(resizeEvent.Width, resizeEvent.Height);
		}
	}

	void Renderer2D::CalculateProjectionMatrix(float width, float height)
	{
		float aspect = width / height;
		m_Projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f);
	}
}