#pragma once

#include "Core/Window.h"
#include "Core/Input.h"
#include "Components/ScriptComponent.h"
#include "Event/Event.h"

//#include "Renderer/Shader.h"

namespace ft {

	class Application
	{
	public:
		Application(const WindowProps& windowProps, const int frameLimit);
		virtual ~Application();

		void Run();
		void Close();
		void OnEvent(Event& event);

		template <class T>
		T* RegisterScriptComponent()
		{
			static_assert(std::is_base_of<ScriptComponent, T>::value, "Registered component must inherit ScriptComponent");
			
			T* script = new T();
			script->SetId(++m_MaxScriptId);
			RegisterInternal(script);
			return script;
		}

		void RemoveScriptComponent(uint16_t componentId);
		void RemoveScriptComponent(ScriptComponent* component);

		static Application& Get() { return *s_Instance; };
		Window& GetWindow() { return *m_Window; };

	private:
		void RegisterInternal(ScriptComponent* scriptComponent);
		void ProcessPendingRemovals();

		bool m_Running = true;
		int m_FrameLimit = -1;

		std::unique_ptr<Window> m_Window;
		std::unique_ptr<Input> m_Input;

		unsigned int m_VertexArray = 0;
		unsigned int m_VertexBuffer = 0;
		unsigned int m_IndexBuffer = 0;
		//std::unique_ptr<Shader> m_Shader;

		static Application* s_Instance;
		
		std::unordered_map<uint16_t, ScriptComponent*> m_ScriptComponents;
		uint16_t m_MaxScriptId = 0;
		std::vector<uint16_t> m_ScriptsToRemove;
	};

	// Should be defined by the client application
	Application* CreateApplication();
}


