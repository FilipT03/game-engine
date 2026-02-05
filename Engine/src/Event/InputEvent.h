#pragma once

#include "Core/Core.h"
#include "Event.h"

namespace ft {

	class KeyEvent : public Event
	{
	public:
		const int key, mods;
	protected:
		KeyEvent(int key, int mods, EventType type) : key(key), mods(mods), Event(type, EventCategory::KeyInput) {};
	};

	class KeyPressEvent : public KeyEvent
	{
	public:
		KeyPressEvent(int key, int mods) : KeyEvent(key, mods, EventType::KeyPress) {}
	};

	class KeyRepeatEvent : public KeyEvent
	{
	public:
		KeyRepeatEvent(int key, int mods) : KeyEvent(key, mods, EventType::KeyRepeat) {}
	};

	class KeyReleaseEvent : public KeyEvent
	{
	public:
		KeyReleaseEvent(int key, int mods) : KeyEvent(key, mods, EventType::KeyRelease) {}
	};



	class MouseEvent : public Event
	{
	protected:
		MouseEvent(EventType type) : Event(type, EventCategory::MouseInput) {};
	};

	class MousePressEvent : public MouseEvent
	{
	public:
		const int button, mods;
		MousePressEvent(int button, int mods) : button(button), mods(mods), MouseEvent(EventType::MousePress) {}
	};

	class MouseReleaseEvent : public MouseEvent
	{
	public:
		const int button, mods;
		MouseReleaseEvent(int button, int mods) : button(button), mods(mods), MouseEvent(EventType::MouseRelease) {}
	};

	class MouseMoveEvent : public MouseEvent
	{
	public:
		const float x, y;
		MouseMoveEvent(float x, float y) : x(x), y(y), MouseEvent(EventType::MouseMove) {}
	};

	class MouseScrollEvent : public MouseEvent
	{
	public:
		const float xDelta, yDelta;
		MouseScrollEvent(float xDelta, float yDelta) : xDelta(xDelta), yDelta(yDelta), MouseEvent(EventType::MouseScroll) {}
	};
}
