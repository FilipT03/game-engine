#pragma once

#include "Core/Core.h"
#include "Renderer/RenderingContext.h"
#include <GLFW/glfw3.h>

namespace ft {

	class OpenGLContext : public RenderingContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle)
			: m_Window(windowHandle) {}
		void Init() override;
		void SwapBuffers() override;
	private:
		GLFWwindow* m_Window;
	};
}

