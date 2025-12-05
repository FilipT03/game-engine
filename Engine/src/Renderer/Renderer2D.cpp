#include "pch.h"
#include "Renderer2D.h"
#include "Renderer/BufferLayout.h"

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

		auto test = BufferLayout({
			{ LayoutElementType::Float3, "a_Position" },
			{ LayoutElementType::Float3, "a_Color" }
			});

		VertexArray* vao = VertexArray::Create(test);

		float vertices[6 * 3] =
		{
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
			 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
		};
		m_VertexBuffer = std::shared_ptr<VertexBuffer>(VertexBuffer::Create(vertices, 6 * 3));
		vao->SetVertexBuffer(m_VertexBuffer);

		unsigned int indices[3] = { 0, 1, 2 };		
		m_IndexBuffer = std::shared_ptr<IndexBuffer>(IndexBuffer::Create(indices, 3));
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

		glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, 0);

		#endif
	}
}