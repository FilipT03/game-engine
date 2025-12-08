#pragma once

#include "Core/Core.h"
#include "Event/InputEvent.h"
#include "GLFW/glfw3.h"
#include "glm/vec2.hpp"

namespace ft {

	class Input
	{
	public:
		void Init(EventCallback eventCallback); 
		static bool IsKeyDown(int key);
		static bool IsCtrlDown();
		static bool IsShiftDown();
		static bool IsAltDown();
		static bool IsMouseDown(int button);
		static glm::vec2 GetMousePosition();
		static glm::vec2 GetMouseDelta();
		static glm::vec2 GetMouseDeltaNormalized();
		
		void OnUpdate();
	private:
		EventCallback m_EventCallback;
		inline static bool s_KeyStates[GLFW_KEY_LAST];
		inline static bool s_MouseStates[GLFW_MOUSE_BUTTON_LAST];
		inline static double s_MouseX = 0;
		inline static double s_MouseY = 0;
		inline static double s_LastMouseX = 0;
		inline static double s_LastMouseY = 0;
		inline static double s_MouseDeltaX = 0;
		inline static double s_MouseDeltaY = 0;
	};
}

