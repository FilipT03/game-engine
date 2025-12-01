#pragma once

#include "Core/Core.h"
#include "Renderer/Buffers.h"
#include "Renderer/Shader.h"

namespace ft {

	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		void Init();
		void Shutdown();
		void Clear();
		void SetClearColor(float r, float g, float b, float a);
		void OnUpdate();
	private:
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;

		unsigned int m_VertexArray = 0;
		std::unique_ptr<Shader> m_Shader;
	};
}

