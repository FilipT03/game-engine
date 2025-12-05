#pragma once

#include "Core/Core.h"
#include "Renderer/Buffers.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"

namespace ft {

	class Renderer2D
	{
	public:
		Renderer2D();
		~Renderer2D();

		void Init();
		void Shutdown();
		void Clear();
		void SetClearColor(float r, float g, float b, float a);
		void OnUpdate();
	private:
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::vector<std::unique_ptr<VertexArray>> m_VertexArrays;

		unsigned int m_VertexArray = 0;
		std::unique_ptr<Shader> m_Shader;
	};
}

