#pragma once

#include "pch.h"
#include "Core/Core.h"

namespace ft {
	enum class EventType
	{
		None, Other,
		WindowClose, WindowResize,
		KeyPress, KeyRepeat, KeyRelease,
		MousePress, MouseRelease, MouseMove, MouseScroll
	};

	enum class EventCategory
	{
		None,
		Window,
		KeyInput,
		MouseInput
	};

	class Event
	{
	public:
		~Event() {}
		const EventType type = EventType::None;
		const EventCategory category = EventCategory::None;

	protected:
		Event() {}
		Event(EventType type, EventCategory category) : type(type), category(category) {}
	};

	typedef std::function<void(Event&)> EventCallback;

	template<typename T>
	inline const T& As(const ft::Event& event)
	{
		return static_cast<const T&>(event);
	}
}

