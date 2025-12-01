#include "pch.h"
#include "Application.h"
#include "Core/Time.h"
#include "Log.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <Core/AssetLoader.h>

namespace ft {
	Application* Application::s_Instance = nullptr;

	Application::Application(const WindowProps& windowProps, const int frameLimit = -1) : m_FrameLimit(frameLimit)
	{
		s_Instance = this;

		m_Window = Window::Create(windowProps, [this](Event& event) { this->OnEvent(event); });

		m_Input = std::make_unique<Input>();
		m_Input->Init([this](Event& event) { this->OnEvent(event); });

		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		glGenBuffers(1, &m_VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

		float vertices[3 * 3] =
		{
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
		};

		unsigned int indices[3] = { 0, 1, 2 };

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		glGenBuffers(1, &m_IndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		const std::string vertexShaderSource = AssetLoader::LoadTextFile("shaders/BasicVertex.glsl");
		const std::string fragmentShaderSource = AssetLoader::LoadTextFile("shaders/BasicFragment.glsl");
		//m_Shader = std::make_unique<Shader>(vertexShaderSource, fragmentShaderSource);
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

			glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			//m_Shader->Bind();
			glBindVertexArray(m_VertexArray);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

			for (auto& it : m_ScriptComponents)
				it.second->OnUpdate();

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