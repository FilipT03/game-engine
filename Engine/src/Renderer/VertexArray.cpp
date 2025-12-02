#include "pch.h"
#include "VertexArray.h"

#ifdef FT_OPENGL_RENDERER
#include "Platform/OpenGL/GLVertexArray.h"
#endif

namespace ft {
	VertexArray* VertexArray::Create(BufferLayout layout)
	{
		#ifdef FT_OPENGL_RENDERER
			return new GLVertexArray(layout);
		#else
			FT_ENGINE_ERROR("Unknown rendering API");
			return nullptr;
		#endif
	}
}