#pragma once

#include "Core/Core.h"
#include "Event/InputEvent.h"
#include "GLFW/glfw3.h"

namespace ft {

	class FT_API Input
	{
	public:
		void Init(EventCallback eventCallback); 
		static bool IsKeyDown(int key);
		static bool IsMouseDown(int button);
		//glm::vec2 GetMousePosition();
		//glm::vec2 GetMouseDelta();
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

