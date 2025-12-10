#pragma once

#include "Core/Core.h"
#include "Event/InputEvent.h"
#include <unordered_map>

namespace ft {

	class Module
	{
	public:
		virtual ~Module() = default;
		virtual void OnUpdate() {}
		virtual void OnRegister() {}
		virtual void OnClose() {}
		virtual void OnDelete() {}

		// Event callbacks
		virtual bool OnEvent(const Event& e) { return false; }
		virtual bool OnKeyEvent(const KeyEvent& e) { return false; }
		virtual bool OnMouseEvent(const MouseEvent& e) { return false; }
		uint16_t GetId() const { return m_Id; }
	private:
		void SetId(uint16_t id) { m_Id = id; }
		uint16_t m_Id = 0;

		friend class Application;
	};
}

