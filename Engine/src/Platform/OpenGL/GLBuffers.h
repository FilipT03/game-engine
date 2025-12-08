#pragma once

#include "Core/Core.h"
#include "Renderer/Buffers.h"

namespace ft {

	class GLVertexBuffer : public VertexBuffer
	{
	public:
		GLVertexBuffer(const float* data, uint32_t size);
		GLVertexBuffer(uint32_t size);
		~GLVertexBuffer();

		void Bind() const override;
		void Unbind() const override;
		void SetData(uint32_t offset, uint32_t size, const void* data) override;

		uint32_t GetCount() const override { return m_Size / sizeof(float); }
		uint32_t GetSize() const override { return m_Size; }

		private:
			uint32_t m_ID;
			uint32_t m_Size;
	};


	class GLIndexBuffer : public IndexBuffer
	{
		public:
		GLIndexBuffer(const uint32_t* data, uint32_t count);
		GLIndexBuffer(uint32_t size);
		~GLIndexBuffer();

		void Bind() const override;
		void Unbind() const override;
		void SetData(uint32_t offset, uint32_t size, const uint32_t* data) override;
		uint32_t GetCount() const override { return m_Count; }

		private:
			uint32_t m_ID;
			uint32_t m_Count;
	};
}

