#include "pch.h"
#include "Window.h"
#include "Core/Log.h"
#include "Platform/OpenGL/GLContext.h"

namespace ft {

	Window::Window(const WindowProps& props, EventCallback eventCallback) : m_Props(props)
	{
		if (props.fullscreen)
		{
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			m_Window = glfwCreateWindow(mode->width, mode->height, props.title.c_str(), monitor, nullptr);
		}
		else
			m_Window = glfwCreateWindow(props.width, props.height, props.title.c_str(), nullptr, nullptr);
		if (m_Window == NULL)
		{
			FT_ENGINE_ERROR("Failed to create GLFW window");
			return;
		}

		#ifdef FT_OPENGL_RENDERER
			m_Context = new GLContext(m_Window);
		#else
			FT_ENGINE_ERROR("Unknown rendering API");
			return;
		#endif
		m_Context->Init();

		m_EventCallback = eventCallback;

		glfwSetWindowUserPointer(m_Window, &m_EventCallback);


		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			EventCallback& callback = *(EventCallback*)glfwGetWindowUserPointer(window);

			WindowCloseEvent event = WindowCloseEvent();
			callback(event);
		});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
			EventCallback& callback = *(EventCallback*)glfwGetWindowUserPointer(window);

			WindowResizeEvent event = WindowResizeEvent(width, height);
			callback(event);
		});
	};

	Window::~Window() 
	{
		if (m_Window)
		{
			glfwDestroyWindow(m_Window);
			glfwTerminate();
		}
		if (m_Context)
		{
			delete m_Context;
			m_Context = nullptr;
		}
	}

	std::unique_ptr<Window> Window::Create(const WindowProps& props, EventCallback eventCallback)
	{
		if (!glfwInit())
		{
			FT_ENGINE_ERROR("Failed to init GLFW");
			return nullptr;
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		return std::make_unique<Window>(props, eventCallback);
	}
	
	void Window::Update()
	{
		m_Context->SwapBuffers();
		glfwPollEvents();
	}
}