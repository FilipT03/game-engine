#include "pch.h"
#include "Input.h"
#include "Core/Log.h"
#include "Core/Window.h"
#include "Core/Application.h"

namespace ft {
	bool Input::IsKeyDown(int key) { return s_KeyStates[key]; }
	bool Input::IsMouseDown(int button) { return s_MouseStates[button]; }
	glm::vec2 Input::GetMousePosition() { return { s_MouseX, s_MouseY }; }
	glm::vec2 Input::GetMouseDelta() { return { s_MouseDeltaX, s_MouseDeltaY }; }

	void Input::Init(EventCallback eventCallback)
	{
		GLFWwindow* window = Application::Get().GetWindow().GetNativeWindow();

		m_EventCallback = eventCallback;
		glfwSetWindowUserPointer(window, &m_EventCallback);

		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			EventCallback& callback = *(EventCallback*)glfwGetWindowUserPointer(window);

			if (action == GLFW_PRESS)
			{
				s_KeyStates[key] = true;
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
				s_KeyStates[key] = false;
				KeyReleaseEvent event = KeyReleaseEvent(key, mods);
				callback(event);
			}
		});


		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
			EventCallback& callback = *(EventCallback*)glfwGetWindowUserPointer(window);

			if (action == GLFW_PRESS)
			{
				s_MouseStates[button] = true;
				MousePressEvent event = MousePressEvent(button, mods);
				callback(event);
			}
			if (action == GLFW_RELEASE)
			{
				s_MouseStates[button] = false;
				MouseReleaseEvent event = MouseReleaseEvent(button, mods);
				callback(event);
			}
		});

		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
			s_MouseDeltaX = xpos - s_MouseX;
			s_MouseDeltaY = ypos - s_MouseY;
			s_MouseX = xpos;
			s_MouseY = ypos;

			EventCallback& callback = *(EventCallback*)glfwGetWindowUserPointer(window);

			MouseMoveEvent event = MouseMoveEvent((float)xpos, (float)ypos);
			callback(event);
		});

		glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
			EventCallback& callback = *(EventCallback*)glfwGetWindowUserPointer(window);

			MouseScrollEvent event = MouseScrollEvent((float)xoffset, (float)yoffset);
			callback(event);
		});
	}
}