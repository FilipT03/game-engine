#pragma once

#include "Core/Core.h"
#include <functional>

namespace ft {

	enum class FT_API EventType
	{
		None, Other,
		WindowClose, WindowResize,
		KeyPress, KeyRepeat, KeyRelease,
		MousePress, MouseRelease, MouseMove, MouseScroll
	};

	enum class FT_API EventCategory
	{
		None,
		Window,
		KeyInput,
		MouseInput
	};

	class FT_API Event
	{
	public:
		~Event() {}
		const EventType Type = EventType::None;
		const EventCategory Category = EventCategory::None;

	protected:
		Event() {}
		Event(EventType type, EventCategory category) : Type(type), Category(category) {}
	};

	typedef std::function<void(Event&)> EventCallback;
}

