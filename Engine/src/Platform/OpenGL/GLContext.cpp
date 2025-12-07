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

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void GLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_Window);
	}
}