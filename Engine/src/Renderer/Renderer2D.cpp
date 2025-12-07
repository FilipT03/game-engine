#include "pch.h"
#include "Renderer2D.h"
#include "Renderer/BufferLayout.h"
#include "Core/Time.h"
#include "Core/Application.h"
#include "Event/WindowEvent.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef FT_OPENGL_RENDERER
#include <glad/glad.h>

#define FT_GLSL_INCLUDE
#include "Platform/OpenGL/Shaders/Basic.vert"
#include "Platform/OpenGL/Shaders/Basic.frag"
#include "Platform/OpenGL/Shaders/Ellipse.frag"
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
			{ LayoutElementType::Float2, "inWorldPosition" },
			{ LayoutElementType::Float2, "inLocalPosition" },
			});

		m_VertexArray = std::unique_ptr<VertexArray>(VertexArray::Create(basicLayout));

		m_VertexBuffer = std::shared_ptr<VertexBuffer>(VertexBuffer::Create(100000));
		m_IndexBuffer = std::shared_ptr<IndexBuffer>(IndexBuffer::Create(100000));

		m_VertexArray->SetVertexBuffer(m_VertexBuffer);
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		m_BasicShader = std::unique_ptr<Shader>(Shader::Create(basicVert, basicFrag));
		m_EllipseShader = std::unique_ptr<Shader>(Shader::Create(basicVert, ellipseFrag));

		// This will be client code
		auto shape1 = AddShape(std::make_shared<Polygon>(4));
		shape1->transform.position.x -= 30.0f;
		shape1->transform.position.y -= 30.0f;
		shape1->UpdateWorldVertices();

		auto shape2 = AddShape(std::make_shared<Polygon>(3));
		shape2->transform.position.x += 30.0f;
		shape2->transform.position.y -= 30.0f;
		shape2->UpdateWorldVertices();

		auto shape3 = AddShape(std::make_shared<Ellipse>());
		shape3->transform.position.y += 30.0f;
		shape3->transform.scale.x = 50;
		shape3->transform.scale.y = 20;
		shape3->color = glm::vec4(0.7f, 0.3f, 0.4f, 1.0f);
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

		uint32_t size = shape->GetVertexByteSize() * 2;
		uint32_t count = shape->GetVertexCount();
		
		shape->vertexByteOffset = m_LastVertexByteOffset;
		m_LastVertexByteOffset += size;

		shape->vertexOffset = m_LastVertexVertexOffset;
		m_LastVertexVertexOffset += count;
		
		std::vector<uint8_t> data;
		PackInterleaved(data, shape->GetVertexCount(), {shape->worldVertices.data(), shape->modelVertices.data()}, m_VertexArray->GetBufferLayout());
		m_VertexBuffer->SetData(shape->vertexByteOffset, size, data.data());

		size = shape->GetIndexByteSize();
		shape->indexOffset = m_LastIndexOffset;
		m_LastIndexOffset += size;
		m_IndexBuffer->SetData(shape->indexOffset, size, shape->indices.data());

		return shape;
	}

	void Renderer2D::OnUpdate()
	{
		#ifdef FT_OPENGL_RENDERER

		m_BasicShader->Bind();
		m_BasicShader->SetUniformMatrix4fv("uProjection", m_Projection);
		m_EllipseShader->Bind();
		m_EllipseShader->SetUniformMatrix4fv("uProjection", m_Projection);
		m_VertexArray->Bind();

		for (auto& shape : m_Shapes)
		{
			// Also client code

			shape->transform.rotation += 2;
			shape->color.r = sin(Time::TotalTime());
			shape->color.g = cos(Time::TotalTime());
			shape->color.b = cos(Time::TotalTime() + glm::pi<float>() / 2.0f);
			shape->UpdateWorldVertices();
			GLenum mode;
			switch (shape->GetType())
			{
				case ShapeType::Ellipse:
				{
					m_EllipseShader->Bind();
					m_EllipseShader->SetUniform4f("uColor", shape->color);
					auto& ellipse = *std::static_pointer_cast<Ellipse>(shape);
					ellipse.thickness = std::abs(sin(Time::TotalTime()));
					m_EllipseShader->SetUniform1f("uThickness", ellipse.thickness);
					m_EllipseShader->SetUniform1f("uAA", ellipse.AA);

					mode = GL_TRIANGLE_FAN;
					break;
				}
			
				case ShapeType::Line:
					mode = GL_LINES;
					break;
			
				default:
					shape->transform.scale.x = sin(Time::TotalTime()) * 20 + 30;
					shape->transform.scale.y = sin(Time::TotalTime()) * 20 + 30;
					shape->UpdateWorldVertices();


					m_BasicShader->Bind();
					m_BasicShader->SetUniform4f("uColor", shape->color);
				
					mode = GL_TRIANGLE_FAN;
					break;
			}

			if (shape->IsDirty())
			{
				std::vector<uint8_t> data;
				PackInterleaved(data, shape->GetVertexCount(), { shape->worldVertices.data(), shape->modelVertices.data() }, m_VertexArray->GetBufferLayout());
				m_VertexBuffer->SetData(shape->vertexByteOffset, shape->GetVertexByteSize() * 2, data.data());
				shape->ResetDirty();
			}

			glDrawElementsBaseVertex(mode, shape->indices.size(), GL_UNSIGNED_INT, (void*)shape->indexOffset, shape->vertexOffset);
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

	/// Sources should be in the same order as the layout.
	void Renderer2D::PackInterleaved(
		std::vector<uint8_t>& out,
		uint32_t vertexCount,
		const std::vector<const void*>& sources,
		const BufferLayout& layout) const
	{
		const auto& elements = layout.GetElements();
		const uint32_t stride = layout.GetStride();

		out.resize(vertexCount * stride);

		uint8_t* destination = out.data();

		for (uint32_t v = 0; v < vertexCount; ++v)
		{
			for (size_t a = 0; a < elements.size(); ++a)
			{
				const auto& element = elements[a];
				const uint8_t* source = (const uint8_t*)sources[a];

				const uint8_t* sourcePtr = source + v * element.Size;
				uint8_t* destinationPtr = destination + element.Offset;

				std::memcpy(destinationPtr, sourcePtr, element.Size);
			}

			destination += stride;
		}
	}
}