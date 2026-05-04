#pragma once

#include "Core/Core.h"
#include "Core/Module.h"

namespace ft {

	class CameraController3D : public Module
	{
	public:
		uint32_t hotkeyMoveForward, hotkeyMoveBackward, hotkeyMoveUp, hotkeyMoveDown, hotkeyMoveLeft, hotkeyMoveRight;
		uint32_t hotkeyZoomMods, hotkeyPanMods, hotkeyMoveMods;

		float moveSpeed = 1.0f, fastModifier = 2.0f, zoomSpeed = 1.0f, panSpeed = 1.0f, rotateSpeed = 1.0f;

		void OnRegister();
		void OnDelete() {};
		void OnUpdate();
		bool OnEvent(const ft::Event& event) { return false; };
		bool OnKeyEvent(const ft::KeyEvent& event);
		bool OnMouseEvent(const ft::MouseEvent& event);

	private:
		bool m_Panning = false;
		bool m_Rotating = false;
		glm::vec3 m_MovementPositive = glm::vec3(0.0f), m_MovementNegative = glm::vec3(0.0f);
		float m_Yaw = 0.0f, m_Pitch = 0.0f;
		float m_BaseMoveSpeed = 10.0f, m_BaseZoomSpeed = 1.0f, m_BasePanSpeed = 10.0f, m_BaseRotateSpeed = 200.0f;
	};
}

