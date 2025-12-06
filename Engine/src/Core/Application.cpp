#include "pch.h"
#include "Application.h"
#include "Core/Time.h"
#include "Log.h"

#include <Core/AssetLoader.h>


namespace ft {
	Application* Application::s_Instance = nullptr;

	Application::Application(const WindowProps& windowProps, const int frameLimit = -1) : m_FrameLimit(frameLimit)
	{
		s_Instance = this;

		m_Window = Window::Create(windowProps, [this](Event& event) { this->OnEvent(event); });

		m_Input = std::make_unique<Input>();
		m_Input->Init([this](Event& event) { this->OnEvent(event); });

		m_Renderer = std::make_unique<Renderer2D>();
		m_Renderer->Init();

		m_Renderer->SetClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	}

	Application::~Application()
	{
		m_Running = false;
		for (auto& it : m_ScriptComponents)
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

			m_Renderer->Clear();


			for (auto& it : m_ScriptComponents)
				it.second->OnUpdate();

			m_Renderer->OnUpdate();

			m_Window->Update();

			ProcessPendingRemovals();
		}
	}

	void Application::ProcessPendingRemovals()
	{
		for (uint16_t id : m_ScriptsToRemove)
		{
			if (!m_ScriptComponents.contains(id))
				continue;
			ScriptComponent* component = m_ScriptComponents.at(id);
			m_ScriptComponents.erase(id);
			component->OnDelete();
			delete component;
		}
		m_ScriptsToRemove.clear();
	}

	void Application::Close()
	{
		m_Running = false;

		for (auto& it : m_ScriptComponents)
			it.second->OnClose();
	}

	void Application::OnEvent(Event& event)
	{
		switch (event.Type)
		{
		case EventType::WindowClose:
			Close();
			break;
		default:
			break;
		}

		m_Renderer->OnEvent(event);
		for (auto& [id, script] : m_ScriptComponents)
		{
			script->OnEvent(event);
			if (event.Category == EventCategory::KeyInput)
				script->OnKeyEvent(static_cast<KeyEvent&>(event));
			if (event.Category == EventCategory::MouseInput)
				script->OnMouseEvent(static_cast<MouseEvent&>(event));
		}
	}

	void Application::RegisterInternal(ScriptComponent* scriptComponent)
	{
		m_ScriptComponents.emplace(scriptComponent->GetId(), scriptComponent);
		scriptComponent->OnRegister();
	}

	void Application::RemoveScriptComponent(uint16_t scriptId)
	{
		m_ScriptsToRemove.push_back(scriptId);
	}

	void Application::RemoveScriptComponent(ScriptComponent* component)
	{
		RemoveScriptComponent(component -> GetId());
	}
}