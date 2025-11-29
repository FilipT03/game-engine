#include "pch.h"
#include "Application.h"
#include "Core/Time.h"
#include "GLFW/glfw3.h"
#include "Log.h"
#include <thread>

namespace ft {
	Application* Application::s_Instance = nullptr;

	Application::Application(const WindowProps& windowProps, const int frameLimit = -1) : m_FrameLimit(frameLimit)
	{
		s_Instance = this;

		m_Window = Window::Create(windowProps, [this](Event& event) { this->OnEvent(event); });

		m_Input = std::make_unique<Input>();
		m_Input->Init([this](Event& event) { this->OnEvent(event); });
	}

	Application::~Application()
	{
		m_Running = false;
		for (auto& it : m_scriptComponents)
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

			for (auto& it : m_scriptComponents)
				it.second->OnUpdate();

			m_Window->Update();

			ProcessPendingRemovals();
		}
	}

	void Application::ProcessPendingRemovals()
	{
		for (uint16_t id : m_ScriptsToRemove)
		{
			if (!m_scriptComponents.contains(id))
				continue;
			ScriptComponent* component = m_scriptComponents.at(id);
			m_scriptComponents.erase(id);
			component->OnDelete();
			delete component;
		}
		m_ScriptsToRemove.clear();
	}

	void Application::Close()
	{
		m_Running = false;

		for (auto& it : m_scriptComponents)
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

		for (auto& [id, script] : m_scriptComponents)
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
		m_scriptComponents.emplace(scriptComponent->GetId(), scriptComponent);
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