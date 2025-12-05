#pragma once

#include "Core/Core.h"

namespace ft {

	class VertexBuffer
	{
	public:
		VertexBuffer() = default;
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual uint32_t GetCount() const = 0;
		virtual uint32_t GetSize() const = 0;

		static VertexBuffer* Create(const float* data, uint32_t count);
		static VertexBuffer* Create(uint32_t size);
	};


	class IndexBuffer 
	{
	public:
		IndexBuffer() = default;
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual uint32_t GetCount() const = 0;

		static IndexBuffer* Create(const uint32_t* data, uint32_t count);
	};
}

