#pragma once

#include "Core/Core.h"
#include "Event.h"

namespace ft {

	class FT_API KeyEvent : public Event
	{
	public:
		const int Key, Mods;
	protected:
		KeyEvent(int key, int mods, EventType type) : Key(key), Mods(mods), Event(type, EventCategory::KeyInput) {};
	};

	class FT_API KeyPressEvent : public KeyEvent
	{
	public:
		KeyPressEvent(int key, int mods) : KeyEvent(key, mods, EventType::KeyPress) {}
	};

	class FT_API KeyRepeatEvent : public KeyEvent
	{
	public:
		KeyRepeatEvent(int key, int mods) : KeyEvent(key, mods, EventType::KeyRepeat) {}
	};

	class FT_API KeyReleaseEvent : public KeyEvent
	{
	public:
		KeyReleaseEvent(int key, int mods) : KeyEvent(key, mods, EventType::KeyRelease) {}
	};



	class FT_API MouseEvent : public Event
	{
	protected:
		MouseEvent(EventType type) : Event(type, EventCategory::MouseInput) {};
	};

	class FT_API MousePressEvent : public MouseEvent
	{
	public:
		const int Button, Mods;
		MousePressEvent(int button, int mods) : Button(button), Mods(mods), MouseEvent(EventType::MousePress) {}
	};

	class FT_API MouseReleaseEvent : public MouseEvent
	{
	public:
		const int Button, Mods;
		MouseReleaseEvent(int button, int mods) : Button(button), Mods(mods), MouseEvent(EventType::MouseRelease) {}
	};

	class FT_API MouseMoveEvent : public MouseEvent
	{
	public:
		const float X, Y;
		MouseMoveEvent(float x, float y) : X(x), Y(y), MouseEvent(EventType::MouseMove) {}
	};

	class FT_API MouseScrollEvent : public MouseEvent
	{
	public:
		const float XDelta, YDelta;
		MouseScrollEvent(float xDelta, float yDelta) : XDelta(xDelta), YDelta(yDelta), MouseEvent(EventType::MouseScroll) {}
	};
}
