#include "SimpleScript.h"

void SimpleScript::OnRegister()
{
	FT_TRACE("Registered SimpleScript");
}

void SimpleScript::OnUpdate()
{
	FT_INFO("Is G pressed? {}", ft::Input::IsKeyDown(GLFW_KEY_G) ? "Yes" : "No");
	//FT_INFO("Update loop, timeDelta is {}", ft::Time::DeltaTime());
	//FT_INFO("Update loop, total time is {0:.3f}s (~{1:.3f}ms)", ft::Time::TotalTime(), ft::Time::DeltaTime()*1000.0);
}

void SimpleScript::OnEvent(const ft::Event& event)
{
	//if (event.Type == ft::EventType::WindowResize)
	//{
	//	auto& resizeEvent = static_cast<const ft::WindowResizeEvent&>(event);
	//	FT_INFO("WindowResize, width = {}, height = {})", resizeEvent.Width, resizeEvent.Height);
	//}
	//if (event.Type == ft::EventType::WindowClose)
	//{
	//	FT_INFO("WindowCloseEvent");
	//}
}

void SimpleScript::OnKeyEvent(const ft::KeyEvent& event)
{
	if (event.Type == ft::EventType::KeyPress)
	{
		auto& pressEvent = ft::As<ft::KeyPressEvent>(event);
		if (event.Key == GLFW_KEY_ESCAPE)
			ft::Application::Get().Close();
	}
	//FT_INFO("Key event: {}, mods: {}", event.Key, event.Mods);
}

void SimpleScript::OnMouseEvent(const ft::MouseEvent& event)
{
	//if (event.Type == ft::EventType::MousePress)
	//{
	//	auto& pressEvent = ft::As<ft::MousePressEvent>(event);
	//	FT_TRACE("Mouse event: {}, mods: {}", pressEvent.Button, pressEvent.Mods);
	//}
}