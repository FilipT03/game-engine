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
			: width(width), height(height), WindowEvent(EventType::WindowResize)
		{}
		const int width = 0, height = 0;
	};
}

