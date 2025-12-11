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
	Renderer2DInternal::Renderer2DInternal(RendererType type) : m_Type(type) {};
	Renderer2DInternal::~Renderer2DInternal() {}

	void Renderer2DInternal::Init()
	{
		#ifdef FT_OPENGL_RENDERER

		if (m_Type == RendererType::World)
			m_Camera = std::make_unique<WorldCamera2D>();
		else
			m_Camera = std::make_unique<UICamera>();
		auto props = Application::Get().GetWindow().GetWindowProps();
		m_Camera->CalculateProjectionMatrix(props.width, props.height);
		m_Camera->RecalculateView();

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
		uint32_t stride = m_VertexArray->GetBufferLayout().GetStride();
		uint32_t count = shape->GetVertexCount();
		uint32_t size = count * stride;
		
		shape->vertexByteOffset = m_LastVertexByteOffset;
		m_LastVertexByteOffset += size;

		shape->vertexOffset = m_LastVertexVertexOffset;
		m_LastVertexVertexOffset += count;
	
		if (shape->worldVertices.size() != count) {
			FT_ENGINE_WARN("World vertices of shape {} not initialized!", shape->GetID());
			shape->UpdateWorldVertices();
		}

		std::vector<uint8_t> data;
		PackInterleaved(data, count, { shape->worldVertices.data(), shape->modelVertices.data()}, m_VertexArray->GetBufferLayout());

		if (data.size() != size) {
			FT_ENGINE_ERROR("Data packing mismatch. Expected {} bytes, got {}", size, data.size());
		}
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

		glDisable(GL_DEPTH_TEST);

		m_BasicShader->Bind();
		m_BasicShader->SetUniformMatrix4fv("uViewProjection", m_Camera->GetViewProjection());
		
		m_EllipseShader->Bind();
		m_EllipseShader->SetUniformMatrix4fv("uViewProjection", m_Camera->GetViewProjection());
		
		m_TextureShader->Bind();
		m_TextureShader->SetUniformMatrix4fv("uViewProjection", m_Camera->GetViewProjection());
		m_TextureShader->SetUniform1i("uTexture", 0);
		
		m_VertexArray->Bind();
		m_VertexBuffer->Bind();
		m_IndexBuffer->Bind();

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

					mode = GL_TRIANGLES;
					break;
				}
			
				case ShapeType::Line:
					m_BasicShader->Bind();

					m_BasicShader->SetUniform4f("uColor", shape->color);

					mode = GL_LINES;
					break;

				case ShapeType::TextureQuad:
				{
					m_TextureShader->Bind();
					m_TextureShader->SetUniform4f("uColor", shape->color);
					int flipY = m_Type == RendererType::UI ? 1 : 0;
					m_TextureShader->SetUniform1i("uFlipY", flipY);
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
				m_VertexBuffer->SetData(shape->vertexByteOffset, shape->GetVertexCount() * m_VertexArray->GetBufferLayout().GetStride(), data.data());
				shape->ResetDirty();
			}

			glDrawElementsBaseVertex(mode, shape->indices.size(), GL_UNSIGNED_INT, (void*)(intptr_t)shape->indexOffset, shape->vertexOffset);
		}
		#endif
	}

	bool Renderer2DInternal::OnEvent(const Event& event)
	{
		if (event.type == EventType::WindowResize)
		{
			glm::vec2 size = Application::Get().GetWindow().GetFrameBufferSize();
			
			#ifdef FT_OPENGL_RENDERER
			glViewport(0, 0, size.x, size.y);
			#endif

			m_Camera->CalculateProjectionMatrix(size.x, size.y);
		}
		return false;
	}


	void Renderer2DInternal::RemoveShape(uint32_t shapeID)
	{
		m_Shapes.erase(shapeID);
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

				const uint8_t* sourcePtr = source + v * element.size;
				uint8_t* destinationPtr = destination + element.offset;

				std::memcpy(destinationPtr, sourcePtr, element.size);
			}

			destination += stride;
		}
	}
}