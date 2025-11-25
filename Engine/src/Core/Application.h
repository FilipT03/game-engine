#pragma once

namespace ft {

	class __declspec(dllexport) Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// Should be defined by the client application
	Application* CreateApplication();
}


