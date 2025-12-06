#pragma once

#include "Core/Core.h"
#include "Event/WindowEvent.h"
#include "Event/InputEvent.h"
#include "Renderer/RenderingContext.h"
#include <GLFW/glfw3.h>

namespace ft {
	struct WindowProps {
		std::string title;
		uint32_t width, height;
		bool fullscreen;

		WindowProps(std::string title = "Application",
			uint32_t width = 1600, uint32_t height = 900, bool fullscreen = false)
			: title(title), width(width), height(height), fullscreen(fullscreen)
		{}
	};

	class Window
	{
	public:
		Window(const WindowProps& props, EventCallback eventCallback);
		~Window();
		
		GLFWwindow* GetNativeWindow() const { return m_Window; }
		WindowProps GetWindowProps() const { return m_Props; }
		static std::unique_ptr<Window> Create(const WindowProps& props, EventCallback eventCallback);
		void Update();

	private:
		GLFWwindow* m_Window;
		WindowProps m_Props;
		EventCallback m_EventCallback;
		RenderingContext* m_Context;
	};
}

