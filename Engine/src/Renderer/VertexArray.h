#pragma once

#include "Core/Core.h"
#include "Renderer/BufferLayout.h"
#include "Renderer/Buffers.h"

namespace ft {

	class VertexArray
	{
	public:
		VertexArray() = default;
		virtual ~VertexArray() = default;
		virtual void Bind() const {}
		virtual void Unbind() const {}
		virtual void SetVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) {}
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) {}

		static VertexArray* Create(BufferLayout layout);
	};
}

