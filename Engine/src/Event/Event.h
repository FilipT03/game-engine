#pragma once

#include "Core/Core.h"
#include <functional>

namespace ft {

	enum FT_API EventType
	{
		WindowClose, WindowResize
	};

	class FT_API Event
	{
	public:
		Event(EventType eventType) : m_EventType(eventType) {}
		~Event();

		EventType GetType() { return m_EventType; }
	private:
		EventType m_EventType;
	};

	typedef std::function<void(Event&)> EventCallback;
}

