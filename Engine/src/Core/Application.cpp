#include "Application.h"
#include "Core/Time.h"
#include "GLFW/glfw3.h"
#include "Log.h"

namespace ft {
	Application* Application::s_Instance = nullptr;

	Application::Application(const WindowProps& windowProps)
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
			it.second->OnDestroy();
			delete it.second;
		}
	}

	void Application::Run()
	{
		while (m_Running)
		{
			double time = glfwGetTime();
			Time::UpdateTime(time);

			for (auto& it : m_scriptComponents)
				it.second->OnUpdate();

			m_Window->Update();
		}
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
}