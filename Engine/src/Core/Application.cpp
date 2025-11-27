#include "Application.h"
#include "Core/Time.h"
#include "GLFW/glfw3.h"
#include "Log.h"

namespace ft {
	Application* Application::s_Instance = nullptr;

	Application::Application(const WindowProps& windowProps)
	{
		m_Window = Window::Create(windowProps);
		s_Instance = this;
	}

	Application::~Application()
	{
		m_Running = false;
	}

	void Application::Run()
	{
		while (m_Running)
		{
			double time = glfwGetTime();
			Time::UpdateTime(time);

			m_Window->Update();
		}
	}

	void Application::Close()
	{
		m_Running = false;
	}

}