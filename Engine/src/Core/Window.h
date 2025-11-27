#pragma once

#include "Core/Core.h"
#include <GLFW/glfw3.h>
#include <string>
#include <memory>

namespace ft {
	struct FT_API WindowProps {
		std::string title;
		uint32_t width, height;

		WindowProps(std::string title = "Application",
			uint32_t width = 1600, uint32_t height = 900)
			: title(title), width(width), height(height)
		{}
	};

	class FT_API Window
	{
	public:
		Window(const WindowProps& props);
		~Window();
		
		GLFWwindow* GetNativeWindow() { return m_Window; }
		static std::unique_ptr<Window> Create(const WindowProps& props);
		void Update();

	private:
		GLFWwindow* m_Window;
		WindowProps m_Props;
	};
}

