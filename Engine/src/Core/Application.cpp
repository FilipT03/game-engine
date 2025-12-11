#include "pch.h"
#include "Application.h"
#include "Core/Time.h"
#include "Core/Log.h"
#include "UI/UIModule.h"
#include "API/UI.h"

namespace ft {
	Application* Application::s_Instance = nullptr;

	Application::Application(const WindowProps& windowProps, const int frameLimit = -1) : m_FrameLimit(frameLimit)
	{
		s_Instance = this;

		m_Window = Window::Create(windowProps, [this](Event& event) { this->OnEvent(event); });

		m_Input = std::make_unique<Input>();
		m_Input->Init([this](Event& event) { this->OnEvent(event); });

		m_WorldRenderer = std::make_unique<Renderer2DInternal>(RendererType::World);
		m_WorldRenderer->Init();

		m_UIRenderer = std::make_unique<Renderer2DInternal>(RendererType::UI);
		m_UIRenderer->Init();

		Renderer2D::s_WorldRenderer = m_WorldRenderer.get();
		Renderer2D::s_UIRenderer = m_UIRenderer.get();

		Renderer2D::SetClearColor(0.7f, 0.7f, 0.7f, 1.0f);

		UIModule* uiModule = RegisterEngineModule<UIModule>(m_UIRenderer.get());
		UI::s_UIModule = uiModule;
	}

	Application::~Application()
	{
		m_Running = false;
		for (auto& it : m_UserModules)
		{
			it.second->OnDelete();
			delete it.second;
		}
		for (auto& it : m_EngineModules)
		{
			it.second->OnDelete();
			delete it.second;
		}
	}

	void Application::Run()
	{
		const double targetFrameTime = 1.0 / m_FrameLimit;
		while (m_Running)
		{
			double time = glfwGetTime();
			
			if (m_FrameLimit > 0)
			{
				double deltaTime = time - Time::TotalTime();
				if (deltaTime < targetFrameTime)
				{
					std::this_thread::sleep_for(std::chrono::duration<double>(targetFrameTime - deltaTime));
					time = glfwGetTime();
				}
			}

			Time::UpdateTime(time);

			Renderer2D::Clear();


			for (auto& it : m_EngineModules) {
				it.second->OnUpdate();
			}

			for (auto& it : m_UserModules)
				it.second->OnUpdate();

			m_Input->OnUpdate();
			
			m_WorldRenderer->OnUpdate();
			m_UIRenderer->OnUpdate();

			m_Window->Update();

			ProcessPendingRemovals();
		}
	}

	void Application::ProcessPendingRemovals()
	{
		for (uint16_t id : m_ModulesToRemove)
		{
			if (m_UserModules.contains(id))
			{
				Module* module = m_UserModules.at(id);
				m_UserModules.erase(id);
				module->OnDelete();
				delete module;
			}
			else if (m_EngineModules.contains(id))
			{
				Module* module = m_EngineModules.at(id);
				m_EngineModules.erase(id);
				module->OnDelete();
				delete module;
			}
		}
		m_ModulesToRemove.clear();
	}

	void Application::Close()
	{
		m_Running = false;

		for (auto& it : m_UserModules)
			it.second->OnClose();
	}

	void Application::OnEvent(Event& event)
	{
		bool stop = false;
		switch (event.type)
		{
		case EventType::WindowClose:
			Close();
			break;
		default:
			break;
		}

		stop = m_UIRenderer->OnEvent(event);
		if (stop) return;
		stop = m_WorldRenderer->OnEvent(event);
		if (stop) return;

		for (auto& [id, module] : m_EngineModules)
		{
			stop = module->OnEvent(event);
			if (stop) return;
			if (event.category == EventCategory::KeyInput)
				stop = module->OnKeyEvent(static_cast<KeyEvent&>(event));
			if (stop) return;
			if (event.category == EventCategory::MouseInput)
				stop = module->OnMouseEvent(static_cast<MouseEvent&>(event));
			if (stop) return;
		}

		for (auto& [id, module] : m_UserModules)
		{
			stop = module->OnEvent(event);
			if (stop) return;
			if (event.category == EventCategory::KeyInput)
				stop = module->OnKeyEvent(static_cast<KeyEvent&>(event));
			if (stop) return;
			if (event.category == EventCategory::MouseInput)
				stop = module->OnMouseEvent(static_cast<MouseEvent&>(event));
			if (stop) return;
		}
	}

	void Application::RegisterInternal(Module* module)
	{
		m_UserModules.emplace(module->GetId(), module);
		module->OnRegister();
	}

	void Application::RegisterEngineInternal(Module* module)
	{
		m_EngineModules.emplace(module->GetId(), module);
		module->OnRegister();
	}

	void Application::RemoveModule(uint16_t moduleId)
	{
		m_ModulesToRemove.push_back(moduleId);
	}

	void Application::RemoveModule(Module* module)
	{
		RemoveModule(module-> GetId());
	}
}