#pragma once

#include "Core/Core.h"
#include "Event.h"

namespace ft {

	class KeyEvent : public Event
	{
	public:
		const int Key, Mods;
	protected:
		KeyEvent(int key, int mods, EventType type) : Key(key), Mods(mods), Event(type, EventCategory::KeyInput) {};
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
		const int Button, Mods;
		MousePressEvent(int button, int mods) : Button(button), Mods(mods), MouseEvent(EventType::MousePress) {}
	};

	class MouseReleaseEvent : public MouseEvent
	{
	public:
		const int Button, Mods;
		MouseReleaseEvent(int button, int mods) : Button(button), Mods(mods), MouseEvent(EventType::MouseRelease) {}
	};

	class MouseMoveEvent : public MouseEvent
	{
	public:
		const float X, Y;
		MouseMoveEvent(float x, float y) : X(x), Y(y), MouseEvent(EventType::MouseMove) {}
	};

	class MouseScrollEvent : public MouseEvent
	{
	public:
		const float XDelta, YDelta;
		MouseScrollEvent(float xDelta, float yDelta) : XDelta(xDelta), YDelta(yDelta), MouseEvent(EventType::MouseScroll) {}
	};
}
