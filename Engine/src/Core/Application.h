#pragma once

#include "Renderer/Shader.h"
#include "Renderer/Renderer2DInternal.h"
#include "API/Renderer2D.h"
#include "Core/Window.h"
#include "Core/Input.h"
#include "Core/Module.h"
#include "Event/Event.h"

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
		T* RegisterModule()
		{
			static_assert(std::is_base_of<Module, T>::value, "Registered module must inherit Module");
			
			T* module = new T();
			module->SetId(++m_MaxModuleId);
			RegisterInternal(module);
			return module;
		}

		template <class T>
		T* RegisterEngineModule()
		{
			static_assert(std::is_base_of<Module, T>::value, "Registered module must inherit Module");

			T* module = new T();
			module->SetId(++m_MaxModuleId);
			RegisterEngineInternal(module);
			return module;
		}

		void RemoveModule(uint16_t moduleId);
		void RemoveModule(Module* module);

		static Application& Get() { return *s_Instance; };
		Window& GetWindow() { return *m_Window; };

	private:
		void RegisterEngineInternal(Module* module);
		void RegisterInternal(Module* module);
		void ProcessPendingRemovals();

		bool m_Running = true;
		int m_FrameLimit = -1;

		std::unique_ptr<Window> m_Window;
		std::unique_ptr<Input> m_Input;
		std::unique_ptr<Renderer2DInternal> m_WorldRenderer, m_UIRenderer;

		static Application* s_Instance;
		
		std::unordered_map<uint16_t, Module*> m_UserModules, m_EngineModules;
		uint16_t m_MaxModuleId = 0;
		std::vector<uint16_t> m_ModulesToRemove;
	};

	// Should be defined by the client application
	Application* CreateApplication();
}


