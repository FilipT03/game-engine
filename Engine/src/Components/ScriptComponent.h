#pragma once

#include "Core/Core.h"
#include "Event/InputEvent.h"
#include <unordered_map>

namespace ft {

	class FT_API ScriptComponent
	{
	public:
		virtual ~ScriptComponent() = default;
		virtual void OnUpdate() {}
		virtual void OnRegister() {}
		virtual void OnClose() {}
		virtual void OnDelete() {}

		// Event callbacks
		virtual void OnEvent(const Event& e) {}
		virtual void OnKeyEvent(const KeyEvent& e) {}
		virtual void OnMouseEvent(const MouseEvent& e) {}
		uint16_t GetId() { return m_Id; }
	private:
		void SetId(uint16_t id) { m_Id = id; }
		uint16_t m_Id = 0;

		friend class Application;
	};
}

