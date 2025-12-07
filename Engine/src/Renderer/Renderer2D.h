#pragma once

#include "Core/Core.h"
#include "Renderer/Buffers.h"
#include "Renderer/BufferLayout.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Shape.h"
#include "Event/Event.h"

#define FT_VIEW_UNITS 100

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
		std::shared_ptr<Shape> AddShape(std::shared_ptr<Shape> shape);

		void OnUpdate();
		void OnEvent(const Event& event);
	private:
		void CalculateProjectionMatrix(float width, float height);
		void PackInterleaved(std::vector<uint8_t>& out, uint32_t vertexCount, const std::vector<const void*>& sources, const BufferLayout& layout) const;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<VertexArray> m_VertexArray;
		std::vector<std::shared_ptr<Shape>> m_Shapes;

		uint32_t m_LastVertexByteOffset = 0, m_LastVertexVertexOffset = 0, m_LastIndexOffset = 0;
		std::unique_ptr<Shader> m_BasicShader, m_EllipseShader;

		glm::mat4 m_Projection;
	};
}

