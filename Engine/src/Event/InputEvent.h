#pragma once

#include "Core/Core.h"
#include "Event.h"

namespace ft {

	class FT_API KeyEvent : public Event
	{
	public:
		const int Key;
	protected:
		KeyEvent(int key, EventType type) : Key(key), Event(type, EventCategory::KeyInput) {};
	};

	class FT_API KeyPressEvent : public KeyEvent
	{
	public:
		KeyPressEvent(int key) : KeyEvent(key, EventType::KeyPress) {}
	};

	class FT_API KeyRepeatEvent : public KeyEvent
	{
	public:
		KeyRepeatEvent(int key) : KeyEvent(key, EventType::KeyRepeat) {}
	};

	class FT_API KeyReleaseEvent : public KeyEvent
	{
	public:
		KeyReleaseEvent(int key) : KeyEvent(key, EventType::KeyRelease) {}
	};



	class FT_API MouseEvent : public Event
	{
	protected:
		MouseEvent(EventType type) : Event(type, EventCategory::MouseInput) {};
	};

	class FT_API MousePressEvent : public MouseEvent
	{
	public:
		const int Button;
		MousePressEvent(int button) : Button(button), MouseEvent(EventType::MousePress) {}
	};

	class FT_API MouseReleaseEvent : public MouseEvent
	{
	public:
		const int Button;
		MouseReleaseEvent(int button) : Button(button), MouseEvent(EventType::MouseRelease) {}
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
		const float Delta;
		MouseScrollEvent(float delta) : Delta(delta), MouseEvent(EventType::MouseScroll) {}
	};
}
