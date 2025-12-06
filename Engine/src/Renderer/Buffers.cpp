#include "pch.h"
#include "Buffers.h"

#ifdef FT_OPENGL_RENDERER
#include "Platform/OpenGL/GLBuffers.h"
#endif

namespace ft {
	VertexBuffer* VertexBuffer::Create(const float* data, uint32_t count) {
		#ifdef FT_OPENGL_RENDERER
			return new GLVertexBuffer(data, count);
		#else
			FT_ENGINE_ERROR("Unknown rendering API");
			return nullptr;
		#endif
	};


	VertexBuffer* VertexBuffer::Create(uint32_t size) {
		#ifdef FT_OPENGL_RENDERER
			return new GLVertexBuffer(size);
		#else
			FT_ENGINE_ERROR("Unknown rendering API");
			return nullptr;
		#endif
	};


	IndexBuffer* IndexBuffer::Create(const uint32_t* data, uint32_t count) {
		#ifdef FT_OPENGL_RENDERER
			return new GLIndexBuffer(data, count);
		#else
			FT_ENGINE_ERROR("Unknown rendering API");
			return nullptr;
		#endif
	};

	IndexBuffer* IndexBuffer::Create(uint32_t size) {
		#ifdef FT_OPENGL_RENDERER
			return new GLIndexBuffer(size);
		#else
			FT_ENGINE_ERROR("Unknown rendering API");
			return nullptr;
		#endif
	};
}