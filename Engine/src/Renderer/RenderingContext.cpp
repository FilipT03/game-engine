#include "pch.h"
#include "RenderingContext.h"
#include "Core/Log.h"

#ifdef FT_OPENGL_RENDERER
#include "Platform/OpenGL/GLContext.h"
#endif

namespace ft {
	RenderingContext* RenderingContext::Create(void* windowHandle)
	{
		#ifdef FT_OPENGL_RENDERER
			return new GLContext(static_cast<GLFWwindow*>(windowHandle));
		#else
			FT_ENGINE_ERROR("Unknown rendering API");
			return nullptr;
		#endif
	}
}