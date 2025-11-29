#pragma once

#include "pch.h"
#include "Core/Core.h"

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

	FT_API typedef std::function<void(Event&)> EventCallback;

	template<typename T>
	inline const T& As(const ft::Event& event)
	{
		return static_cast<const T&>(event);
	}
}

