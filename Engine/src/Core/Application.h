#pragma once

#include <memory>
#include "Core/Window.h"
#include "Core/Timer.h"

namespace ft {

	class FT_API Application
	{
	public:
		Application(const WindowProps& windowProps);
		virtual ~Application();

		void Run();
		void Close();

		static Application& Get() { return *s_Instance; };
		Window& GetWindow() { return *m_Window; };

	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		Timer m_timer;
		
		static Application* s_Instance;
	};

	// Should be defined by the client application
	Application* CreateApplication();
}


