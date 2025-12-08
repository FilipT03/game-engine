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
	struct Camera2D
	{
		glm::vec2 position = { 0.0f, 00.0f };
		float zoom = 1.0f;
	};

	class Renderer2DInternal
	{
	public:
		Renderer2DInternal();
		~Renderer2DInternal();

		void Init();
		void Shutdown();
		
		void Clear();
		void SetClearColor(float r, float g, float b, float a);

		template <typename ShapeType, typename... Args>
		Shape* AddShape(Args&&... args)
		{
			static_assert(std::is_base_of<Shape, ShapeType>::value, "Shape type must be derived from Shape");
			auto shape = std::make_unique<ShapeType>(std::forward<Args>(args)...);
			m_Shapes.push_back(std::move(shape));
			return AddShapeInternal(m_Shapes.back().get());
		}
		void RecalculateView();
		Camera2D* GetCamera() { return &m_Camera; };
		glm::vec2 ScreenToWorld(glm::vec2 screenCoordinates) const;

		void OnUpdate();
		void OnEvent(const Event& event);
	private:
		Shape* AddShapeInternal(Shape* shape);
		void CalculateProjectionMatrix(float width, float height);
		void PackInterleaved(std::vector<uint8_t>& out, uint32_t vertexCount, const std::vector<const void*>& sources, const BufferLayout& layout) const;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<VertexArray> m_VertexArray;
		std::vector<std::unique_ptr<Shape>> m_Shapes;
		uint32_t m_LastShapeId = 1;

		uint32_t m_LastVertexByteOffset = 0, m_LastVertexVertexOffset = 0, m_LastIndexOffset = 0;
		std::unique_ptr<Shader> m_BasicShader, m_EllipseShader;

		Camera2D m_Camera;
		glm::mat4 m_View;
		glm::mat4 m_Projection;
		glm::mat4 m_ViewProjection;
	};
}

