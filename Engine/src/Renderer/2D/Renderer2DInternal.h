#pragma once

#include "Core/Core.h"
#include "Renderer/Buffers.h"
#include "Renderer/BufferLayout.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Renderer/2D/Shape.h"
#include "Renderer/Camera.h"
#include "Event/Event.h"

namespace ft {
	enum class RendererType {
		World, UI
	};

	class Renderer2DInternal
	{
	public:
		Renderer2DInternal(RendererType type);
		~Renderer2DInternal();

		void Init();
		void Shutdown();
		
		static void Clear();
		static void SetClearColor(float r, float g, float b, float a);

		template <typename ShapeType, typename... Args>
		Shape* AddShape(Args&&... args)
		{
			static_assert(std::is_base_of<Shape, ShapeType>::value, "Shape type must be derived from Shape");
			auto shape = std::make_unique<ShapeType>(std::forward<Args>(args)...);
			shape->SetID(m_LastShapeId++);
			auto [it, success] = m_Shapes.insert({ shape->GetID(), std::move(shape) });
			return AddShapeInternal(it->second.get());
		}
		void RemoveShape(uint32_t shapeID);
		Camera* GetCamera() { return m_Camera.get(); };

		void OnUpdate();
		bool OnEvent(const Event& event);
	private:
		Shape* AddShapeInternal(Shape* shape);
		void PackInterleaved(std::vector<uint8_t>& out, uint32_t vertexCount, const std::vector<const void*>& sources, const BufferLayout& layout) const;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<VertexArray> m_VertexArray;
		std::map<uint32_t, std::unique_ptr<Shape>> m_Shapes;
		uint32_t m_LastShapeId = 1;

		uint32_t m_LastVertexByteOffset = 0, m_LastVertexVertexOffset = 0, m_LastIndexOffset = 0;
		std::unique_ptr<Shader> m_BasicShader, m_EllipseShader, m_TextureShader;

		RendererType m_Type;
		std::unique_ptr<Camera> m_Camera;
	};
}
