#include "pch.h"
#include "GLContext.h"

#include "Core/Log.h"
#include <glad/glad.h>

namespace ft {
	void GLContext::Init()
	{
		glfwMakeContextCurrent(m_Window);
		glfwSwapInterval(0); // TODO: VSync

		int success = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		if (!success)
			FT_ENGINE_ERROR("Failed to init GLAD");
	}

	void GLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_Window);
	}
}