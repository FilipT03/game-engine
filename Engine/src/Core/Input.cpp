#include "Input.h"
#include "Core/Log.h"
#include "Core/Window.h"
#include "Core/Application.h"

namespace ft {
	void Input::Init(EventCallback eventCallback)
	{
		GLFWwindow* window = Application::Get().GetWindow().GetNativeWindow();

		m_EventCallback = eventCallback;
		glfwSetWindowUserPointer(window, &m_EventCallback);


		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			EventCallback& callback = *(EventCallback*)glfwGetWindowUserPointer(window);

			if (action == GLFW_PRESS)
			{
				KeyPressEvent event = KeyPressEvent(key, mods);
				callback(event);
			}
			if (action == GLFW_REPEAT)
			{
				KeyRepeatEvent event = KeyRepeatEvent(key, mods);
				callback(event);
			}
			if (action == GLFW_RELEASE)
			{
				KeyReleaseEvent event = KeyReleaseEvent(key, mods);
				callback(event);
			}
		});


		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
			EventCallback& callback = *(EventCallback*)glfwGetWindowUserPointer(window);

			if (action == GLFW_PRESS)
			{
				MousePressEvent event = MousePressEvent(button, mods);
				callback(event);
			}
			if (action == GLFW_RELEASE)
			{
				MouseReleaseEvent event = MouseReleaseEvent(button, mods);
				callback(event);
			}
		});

		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
			EventCallback& callback = *(EventCallback*)glfwGetWindowUserPointer(window);

			MouseMoveEvent event = MouseMoveEvent(xpos, ypos);
			callback(event);
		});

		glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
			EventCallback& callback = *(EventCallback*)glfwGetWindowUserPointer(window);

			MouseScrollEvent event = MouseScrollEvent(xoffset, yoffset);
			callback(event);
		});
	}
}