#pragma once

#include "Core/Core.h"
#include "Renderer/Buffers.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Shape.h"
#include "Event/Event.h"

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
		void OnEvent(const Event& event);
	private:
		void CalculateProjectionMatrix(float width, float height);
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::vector<std::unique_ptr<VertexArray>> m_VertexArrays;
		std::vector<std::unique_ptr<Shape>> m_Shapes;

		uint32_t m_LastVertexOffset = 0, m_LastIndexOffset = 0;
		std::unique_ptr<Shader> m_Shader;

		glm::mat4 m_Projection;
	};
}

