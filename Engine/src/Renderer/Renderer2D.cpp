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
	Renderer2D::Renderer2D() : m_Projection(0.0f) {};
	Renderer2D::~Renderer2D() {}

	void Renderer2D::Init()
	{
		#ifdef FT_OPENGL_RENDERER

		auto props = Application::Get().GetWindow().GetWindowProps();
		CalculateProjectionMatrix(props.width, props.height);

		auto basicLayout = BufferLayout({
			{ LayoutElementType::Float2, "inPosition" },
			});

		m_VertexArray = std::unique_ptr<VertexArray>(VertexArray::Create(basicLayout));

		m_VertexBuffer = std::shared_ptr<VertexBuffer>(VertexBuffer::Create(100000));
		m_IndexBuffer = std::shared_ptr<IndexBuffer>(IndexBuffer::Create(100000));

		m_VertexArray->SetVertexBuffer(m_VertexBuffer);
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		m_BasicShader = std::unique_ptr<Shader>(Shader::Create(basicVert, basicFrag));
		m_BasicShader->Bind();

		// This will be client code
		auto shape1 = AddShape(std::make_shared<Polygon>(4));
		shape1->transform.position.x -= 30.0f;
		shape1->transform.position.y -= 30.0f;
		shape1->UpdateWorldVertices();

		auto shape2 = AddShape(std::make_shared<Polygon>(3));
		shape2->transform.position.x += 30.0f;
		shape2->transform.position.y -= 30.0f;
		shape2->UpdateWorldVertices();

		auto shape3 = AddShape(std::make_shared<Polygon>(7));
		shape3->transform.position.y += 30.0f;
		shape3->UpdateWorldVertices();
		//AddShape(Line(glm::vec2{-0.4,0}, glm::vec2{ +0.4,0 }));

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

	std::shared_ptr<Shape> Renderer2D::AddShape(std::shared_ptr<Shape> shape)
	{
		m_Shapes.push_back(shape);

		uint32_t size = shape->GetVertexByteSize();
		uint32_t count = shape->GetVertexCount();
		
		shape->vertexByteOffset = m_LastVertexByteOffset;
		m_LastVertexByteOffset += size;

		shape->vertexOffset = m_LastVertexVertexOffset;
		m_LastVertexVertexOffset += count;
		
		m_VertexBuffer->SetData(shape->vertexByteOffset, size, shape->worldVertices.data());

		size = shape->GetIndexByteSize();
		shape->indexOffset = m_LastIndexOffset;
		m_LastIndexOffset += size;
		m_IndexBuffer->SetData(shape->indexOffset, size, shape->indices.data());

		return shape;
	}

	void Renderer2D::OnUpdate()
	{
		#ifdef FT_OPENGL_RENDERER

		m_BasicShader->SetUniformMatrix4fv("uProjection", m_Projection);
		m_VertexArray->Bind();

		for (auto& shape : m_Shapes)
		{
			// Also client code
			shape->transform.rotation += 2;
			shape->color.r = sin(Time::TotalTime());
			shape->color.g = cos(Time::TotalTime());
			shape->color.b = cos(Time::TotalTime() + glm::pi<float>() / 2.0f);
			shape->transform.scale.x = sin(Time::TotalTime()) / 2.0 + 30;
			shape->transform.scale.y = sin(Time::TotalTime()) / 2.0 + 30;
			shape->UpdateWorldVertices();
			
			m_BasicShader->SetUniform3f("uColor", shape->color);

			if (shape->IsDirty())
			{
				m_VertexBuffer->SetData(shape->vertexByteOffset, shape->GetVertexByteSize(), shape->worldVertices.data());
				shape->ResetDirty();
			}

			glDrawElementsBaseVertex(GL_TRIANGLE_FAN, shape->indices.size(), GL_UNSIGNED_INT, (void*)shape->indexOffset, shape->vertexOffset);
		}
		#endif
	}

	void Renderer2D::OnEvent(const Event& event)
	{
		if (event.Type == EventType::WindowResize)
		{
			glm::vec2 size = Application::Get().GetWindow().GetFrameBufferSize();
			
			#ifdef FT_OPENGL_RENDERER
			glViewport(0, 0, size.x, size.y);
			#endif

			CalculateProjectionMatrix(size.x, size.y);
		}
	}

	/// The smaller dimension will be 100 units, and the larger will be 100 x aspect ratio.
	void Renderer2D::CalculateProjectionMatrix(float width, float height)
	{
		float aspect;
		if (width > height)
		{
			aspect = width / height;
			m_Projection = glm::ortho(
				-aspect * 0.5f * FT_VIEW_UNITS, aspect * 0.5f * FT_VIEW_UNITS,
						 -0.5f * FT_VIEW_UNITS,			 0.5f * FT_VIEW_UNITS);
		}
		else
		{
			aspect = height / width;
			m_Projection = glm::ortho(
						 -0.5f * FT_VIEW_UNITS,			 0.5f * FT_VIEW_UNITS,
				-aspect * 0.5f * FT_VIEW_UNITS, aspect * 0.5f * FT_VIEW_UNITS);

		}
	}
}