#include "SimpleScript.h"

void SimpleScript::OnRegister()
{
	FT_TRACE("Registered SimpleScript");
}

void SimpleScript::OnUpdate()
{
	//FT_INFO("Update loop, timeDelta is {}", ft::Time::DeltaTime());
	//FT_INFO("Update loop, total time is {0:.3f}s (~{1:.3f}ms)", ft::Time::TotalTime(), ft::Time::DeltaTime()*1000.0);
}

void SimpleScript::OnEvent(const ft::Event& event)
{
	if (event.Type == ft::EventType::WindowResize)
	{
		auto& resizeEvent = static_cast<const ft::WindowResizeEvent&>(event);
		FT_INFO("WindowResize, width = {}, height = {})", resizeEvent.Width, resizeEvent.Height);
	}
	if (event.Type == ft::EventType::WindowClose)
	{
		FT_INFO("WindowCloseEvent");
	}
}