#pragma once

#include "Core/Core.h"
#include "Event.h"

namespace ft {

	class FT_API WindowEvent : public Event
	{
	protected:
		WindowEvent(EventType type) : Event(type, EventCategory::Window) {}
	};

	class FT_API WindowCloseEvent : public WindowEvent
	{
	public:
		WindowCloseEvent() : WindowEvent(EventType::WindowClose) {}
	};

	class FT_API WindowResizeEvent : public WindowEvent
	{
	public:
		WindowResizeEvent(int width, int height) 
			: Width(width), Height(height), WindowEvent(EventType::WindowResize)
		{}
		const int Width = 0, Height = 0;
	};
}

