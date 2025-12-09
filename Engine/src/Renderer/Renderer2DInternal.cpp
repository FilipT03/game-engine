#include "pch.h"
#include "Renderer2DInternal.h"
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
#include "Platform/OpenGL/Shaders/Texture.frag"
#endif

namespace ft {
	Renderer2DInternal::Renderer2DInternal() : m_Projection(0.0f), m_View(0.0f), m_ViewProjection(0.0f) {};
	Renderer2DInternal::~Renderer2DInternal() {}

	void Renderer2DInternal::Init()
	{
		#ifdef FT_OPENGL_RENDERER

		auto props = Application::Get().GetWindow().GetWindowProps();
		CalculateProjectionMatrix(props.width, props.height);
		RecalculateView();

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
		m_TextureShader = std::unique_ptr<Shader>(Shader::Create(basicVert, textureFrag));

		#endif
	}

	void Renderer2DInternal::Shutdown()
	{
		#ifdef FT_OPENGL_RENDERER
		#endif
	}

	void Renderer2DInternal::Clear()
	{
		#ifdef FT_OPENGL_RENDERER
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		#endif
	}

	void Renderer2DInternal::SetClearColor(float r, float g, float b, float a)
	{
		#ifdef FT_OPENGL_RENDERER
		glClearColor(r, g, b, a);
		#endif
	}

	Shape* Renderer2DInternal::AddShapeInternal(Shape* shape)
	{
		uint32_t size = shape->GetVertexByteSize() * 2;
		uint32_t count = shape->GetVertexCount();
		
		shape->vertexByteOffset = m_LastVertexByteOffset;
		m_LastVertexByteOffset += size;

		shape->vertexOffset = m_LastVertexVertexOffset;
		m_LastVertexVertexOffset += count;
		
		std::vector<uint8_t> data;
		PackInterleaved(data, shape->GetVertexCount(), { shape->worldVertices.data(), shape->modelVertices.data()}, m_VertexArray->GetBufferLayout());
		m_VertexBuffer->SetData(shape->vertexByteOffset, size, data.data());

		size = shape->GetIndexByteSize();
		shape->indexOffset = m_LastIndexOffset;
		m_LastIndexOffset += size;
		m_IndexBuffer->SetData(shape->indexOffset, size, shape->indices.data());

		return shape;
	}

	void Renderer2DInternal::OnUpdate()
	{
		#ifdef FT_OPENGL_RENDERER

		m_BasicShader->Bind();
		m_BasicShader->SetUniformMatrix4fv("uViewProjection", m_ViewProjection);
		
		m_EllipseShader->Bind();
		m_EllipseShader->SetUniformMatrix4fv("uViewProjection", m_ViewProjection);
		
		m_TextureShader->Bind();
		m_TextureShader->SetUniformMatrix4fv("uViewProjection", m_ViewProjection);
		m_TextureShader->SetUniform1i("uTexture", 0);
		
		m_VertexArray->Bind();

		for (auto& [id, shape] : m_Shapes)
		{
			GLenum mode;
			switch (shape->GetType())
			{
				case ShapeType::Ellipse:
				{
					m_EllipseShader->Bind();
					m_EllipseShader->SetUniform4f("uColor", shape->color);

					Ellipse* ellipse = dynamic_cast<Ellipse*>(shape.get());
					m_EllipseShader->SetUniform1f("uThickness", ellipse->thickness);
					m_EllipseShader->SetUniform1f("uAA", ellipse->AA);

					mode = GL_TRIANGLE_FAN;
					break;
				}
			
				case ShapeType::Line:
					mode = GL_LINES;
					break;

				case ShapeType::TextureQuad:
				{
					m_TextureShader->Bind();
					TextureQuad* textureQuad = dynamic_cast<TextureQuad*>(shape.get());
					textureQuad->GetTexture()->Bind(0);

					mode = GL_TRIANGLES;
					break;
				}
				default:
					m_BasicShader->Bind();

					m_BasicShader->SetUniform4f("uColor", shape->color);
				
					mode = GL_TRIANGLES;
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

	void Renderer2DInternal::OnEvent(const Event& event)
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
	void Renderer2DInternal::CalculateProjectionMatrix(float width, float height)
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

		m_ViewProjection = m_Projection * m_View;
	}

	void Renderer2DInternal::RemoveShape(uint32_t shapeID)
	{
		m_Shapes.erase(shapeID);
	}

	void Renderer2DInternal::RecalculateView()
	{
		m_View = glm::translate(glm::mat4(1.0f), glm::vec3(-m_Camera.position, 0.0f));
		m_View = glm::scale(m_View, glm::vec3(m_Camera.zoom, m_Camera.zoom, 1.0f));

		m_ViewProjection = m_Projection * m_View;
	}

	glm::vec2 Renderer2DInternal::ScreenToWorld(glm::vec2 screenCoordinates) const
	{
		glm::vec2 windowSize = Application::Get().GetWindow().GetWindowSize();
		glm::vec2 ndc{};
		ndc.x = (screenCoordinates.x / windowSize.x) * 2.0f - 1.0f;
		ndc.y = 1.0f - (screenCoordinates.y / windowSize.y) * 2.0f;

		glm::mat4 invVP = glm::inverse(m_ViewProjection);
		glm::vec4 world = invVP * glm::vec4(ndc.x, ndc.y, 0.0f, 1.0f);
		return glm::vec2(world.x, world.y);
	}

	glm::vec2 Renderer2DInternal::WorldToScreen(glm::vec2 worldCoordinates) const
	{
		glm::vec4 ndc = m_ViewProjection * glm::vec4(worldCoordinates.x, worldCoordinates.y, 0.0f, 1.0f);
		
		glm::vec2 windowSize = Application::Get().GetWindow().GetWindowSize();
		glm::vec2 screenCoords{};
		ndc = (ndc + 1.0f) / 2.0f; // transform to range [0,1]
		screenCoords.x = windowSize.x * ndc.x;
		screenCoords.y = windowSize.y * (1.0f - ndc.y);
		return screenCoords;
	}

	/// Sources should be in the same order as the layout.
	void Renderer2DInternal::PackInterleaved(
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