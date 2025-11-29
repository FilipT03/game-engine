#include "SimpleScript.h"

void SimpleScript::OnRegister()
{
	FT_TRACE("Registered SimpleScript");
}

void SimpleScript::OnDelete()
{
	FT_TRACE("Removed SimpleScript");
}

void SimpleScript::OnUpdate()
{
	double fps = 1 / ft::Time::DeltaTime();
	FT_INFO(fps);
}

void SimpleScript::OnEvent(const ft::Event& event)
{
}

void SimpleScript::OnKeyEvent(const ft::KeyEvent& event)
{
	if (event.Type == ft::EventType::KeyPress && event.Key == GLFW_KEY_ESCAPE)
		ft::Application::Get().Close();
	if (event.Type == ft::EventType::KeyPress && event.Key == GLFW_KEY_G)
		ft::Application::Get().RemoveScriptComponent(GetId());
}

void SimpleScript::OnMouseEvent(const ft::MouseEvent& event)
{
	//if (event.Type == ft::EventType::MousePress)
	//{
	//	auto& pressEvent = ft::As<ft::MousePressEvent>(event);
	//	FT_TRACE("Mouse event: {}, mods: {}", pressEvent.Button, pressEvent.Mods);
	//}
}