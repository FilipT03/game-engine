#include "pch.h"
#include "Renderer.h"
#include "Renderer/BufferLayout.h"

#ifdef FT_OPENGL_RENDERER
#include <glad/glad.h>

#define FT_GLSL_INCLUDE
#include "Platform/OpenGL/Shaders/Basic.vert"
#include "Platform/OpenGL/Shaders/Basic.frag"
#endif

namespace ft {
	Renderer::Renderer() {};
	Renderer::~Renderer() {}

	void Renderer::Init()
	{
		#ifdef FT_OPENGL_RENDERER
		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);


		float vertices[6 * 3] =
		{
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
			 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
		};

		m_VertexBuffer = std::unique_ptr<VertexBuffer>(VertexBuffer::Create(vertices, 6 * 3));
		m_VertexBuffer->Bind();

		unsigned int indices[3] = { 0, 1, 2 };
		
		m_IndexBuffer = std::unique_ptr<IndexBuffer>(IndexBuffer::Create(indices, 3));
		m_IndexBuffer->Bind();

		auto test = BufferLayout({
			{ LayoutElementType::Float3, "a_Position" },
			{ LayoutElementType::Float4, "a_Color" },
			{ LayoutElementType::Float3, "a_dColor" }
			});

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));


		m_Shader = std::unique_ptr<Shader>(Shader::Create(basicVert, basicFrag));
		m_Shader->Bind();

		#endif
	}

	void Renderer::Shutdown()
	{
		#ifdef FT_OPENGL_RENDERER

		glDeleteVertexArrays(1, &m_VertexArray);

		#endif
	}

	void Renderer::Clear()
	{
		#ifdef FT_OPENGL_RENDERER
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		#endif
	}

	void Renderer::SetClearColor(float r, float g, float b, float a)
	{
		#ifdef FT_OPENGL_RENDERER
		glClearColor(r, g, b, a);
		#endif
	}

	void Renderer::OnUpdate()
	{
		#ifdef FT_OPENGL_RENDERER

		glBindVertexArray(m_VertexArray);
		glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, 0);

		#endif
	}
}