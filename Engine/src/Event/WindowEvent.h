#pragma once

#include "Core/Core.h"
#include "Event.h"

namespace ft {

	class WindowEvent : public Event
	{
	protected:
		WindowEvent(EventType type) : Event(type, EventCategory::Window) {}
	};

	class WindowCloseEvent : public WindowEvent
	{
	public:
		WindowCloseEvent() : WindowEvent(EventType::WindowClose) {}
	};

	class WindowResizeEvent : public WindowEvent
	{
	public:
		WindowResizeEvent(int width, int height) 
			: Width(width), Height(height), WindowEvent(EventType::WindowResize)
		{}
		const int Width = 0, Height = 0;
	};
}

