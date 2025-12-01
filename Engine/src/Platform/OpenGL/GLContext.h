#pragma once

#include "Core/Core.h"
#include "Renderer/RenderingContext.h"
#include <GLFW/glfw3.h>

namespace ft {

	class GLContext : public RenderingContext
	{
	public:
		GLContext(GLFWwindow* windowHandle)
			: m_Window(windowHandle) {}
		void Init() override;
		void SwapBuffers() override;
	private:
		GLFWwindow* m_Window;
	};
}

