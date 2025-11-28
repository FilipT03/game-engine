#pragma once

#include <memory>
#include <vector>
#include "Core/Window.h"
#include "Core/Timer.h"
#include "Components/ScriptComponent.h"

namespace ft {

	class FT_API Application
	{
	public:
		Application(const WindowProps& windowProps);
		virtual ~Application();

		void Run();
		void Close();

		template <class T>
		void RegisterScriptComponent()
		{
			static_assert(std::is_base_of<ScriptComponent, T>::value, "Registered component must inherit ScriptComponent");
			
			T* script = new T();
			script->SetId(++m_maxScriptId);
			RegisterInternal(script);
		}

		static Application& Get() { return *s_Instance; };
		Window& GetWindow() { return *m_Window; };

	private:
		void RegisterInternal(ScriptComponent* scriptComponent);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		Timer m_timer;

		static Application* s_Instance;
		
		std::unordered_map<uint16_t, ScriptComponent*> m_scriptComponents;
		uint16_t m_maxScriptId = 0;
	};

	// Should be defined by the client application
	Application* CreateApplication();
}


