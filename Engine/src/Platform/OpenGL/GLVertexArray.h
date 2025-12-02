#pragma once

#include "Core/Core.h"
#include "Renderer/VertexArray.h"

namespace ft {

	class GLVertexArray : public VertexArray
	{
	public:
		GLVertexArray(BufferLayout layout);
		~GLVertexArray();
		void Bind() const override;
		void Unbind() const override;
		void SetVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
		void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

	private:
		uint32_t m_ID;
		BufferLayout m_Layout;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		uint32_t m_Offset = 0;
	};
}

