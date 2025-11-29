#pragma once

#include <memory>
#include <vector>
#include "Core/Window.h"
#include <Core/Input.h>
#include "Components/ScriptComponent.h"
#include <Event/Event.h>

namespace ft {

	class FT_API Application
	{
	public:
		Application(const WindowProps& windowProps, const int frameLimit);
		virtual ~Application();

		void Run();
		void Close();
		void OnEvent(Event& event);

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

		bool m_Running = true;
		int m_FrameLimit = -1;

		std::unique_ptr<Window> m_Window;
		std::unique_ptr<Input> m_Input;

		static Application* s_Instance;
		
		std::unordered_map<uint16_t, ScriptComponent*> m_scriptComponents;
		uint16_t m_maxScriptId = 0;
	};

	// Should be defined by the client application
	Application* CreateApplication();
}


