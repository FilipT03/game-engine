#include "Window.h"
#include "Core/Log.h"

namespace ft {

	Window::Window(const WindowProps& props, EventCallback eventCallback) : m_Props(props)
	{
		m_Window = glfwCreateWindow(props.width, props.height, props.title.c_str(), nullptr, nullptr);
		if (m_Window == NULL)
		{
			FT_CORE_ERROR("Failed to create GLFW window");
			return;
		}

		m_EventCallback = eventCallback;

		glfwSetWindowUserPointer(m_Window, &m_EventCallback);

		glfwMakeContextCurrent(m_Window);
		//glfwSwapInterval(1); // TODO: VSync

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			EventCallback& callback = *(EventCallback*)glfwGetWindowUserPointer(window);

			Event event = Event(EventType::WindowClose);
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
	}

	std::unique_ptr<Window> Window::Create(const WindowProps& props, EventCallback eventCallback)
	{
		if (!glfwInit())
		{
			FT_CORE_ERROR("Failed to init GLFW");
			return nullptr;
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		return std::make_unique<Window>(props, eventCallback);
	}
	
	void Window::Update()
	{
		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}
}