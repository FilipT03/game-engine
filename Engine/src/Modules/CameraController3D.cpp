#include "pch.h"
#include "CameraController3D.h"
#include "Core/Input.h"
#include "Core/Time.h"
#include "Event/Event.h"
#include "API/Renderer3D.h"
#include <GLFW/glfw3.h>

namespace ft {
	void CameraController3D::OnRegister() {
		hotkeyMoveForward = GLFW_KEY_W;
		hotkeyMoveBackward = GLFW_KEY_S;
		hotkeyMoveUp = GLFW_KEY_E;
		hotkeyMoveDown = GLFW_KEY_Q;
		hotkeyMoveLeft = GLFW_KEY_A;
		hotkeyMoveRight = GLFW_KEY_D;
		hotkeyZoomMods = 0;
		hotkeyPanMods = GLFW_MOD_SHIFT;
		hotkeyMoveMods = GLFW_MOD_CONTROL;
	}

	void CameraController3D::OnUpdate() {
		m_Pitch = glm::degrees(asin(Renderer3D::GetCamera()->GetFront().y));
		m_Yaw = glm::degrees(atan2(Renderer3D::GetCamera()->GetFront().z, Renderer3D::GetCamera()->GetFront().x));
		if (m_Panning) {
			WorldCamera3D* camera = Renderer3D::GetCamera();
			glm::vec2 mouseDelta = Input::GetMouseDeltaNormalized();

			float speed = panSpeed * m_BasePanSpeed;
			glm::vec3 offset = -camera->GetRight() * mouseDelta.x * speed + camera->GetUp() * mouseDelta.y * speed;
			camera->Translate(offset);
		}
		if (m_Rotating) {
			WorldCamera3D* camera = Renderer3D::GetCamera();
			glm::vec2 mouseDelta = Input::GetMouseDeltaNormalized();
			mouseDelta *= rotateSpeed * m_BaseRotateSpeed;

			m_Yaw += mouseDelta.x;
			m_Pitch -= mouseDelta.y;

			if (m_Pitch > 89.0f)
				m_Pitch = 89.0f;
			if (m_Pitch < -89.0f)
				m_Pitch = -89.0f;
			glm::vec3 direction;
			direction.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
			direction.y = sin(glm::radians(m_Pitch));
			direction.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
			camera->SetFront(glm::normalize(direction));
		}
		if (glm::length(m_MovementPositive + m_MovementNegative) > 0.0001f) {
			glm::vec3 movement = m_MovementPositive + m_MovementNegative;
			movement = glm::normalize(movement);
			float totalMoveSpeed = moveSpeed * m_BaseMoveSpeed * Time::DeltaTimeF() * (Input::IsShiftDown() ? fastModifier : 1.0f);
			WorldCamera3D* camera = Renderer3D::GetCamera();
			glm::vec3 offset = (camera->GetFront() * movement.z + 
								camera->GetUp()    * movement.y + 
								camera->GetRight() * movement.x) * totalMoveSpeed;
			camera->Translate(offset);
		}
	}

	bool CameraController3D::OnKeyEvent(const ft::KeyEvent& event) {
		WorldCamera3D* camera = Renderer3D::GetCamera();
		if (event.type == EventType::KeyPress) {
			if (event.key == hotkeyMoveForward && event.mods == hotkeyMoveMods)
				m_MovementPositive.z = 1;
			if (event.key == hotkeyMoveBackward && event.mods == hotkeyMoveMods)
				m_MovementNegative.z = -1;
			if (event.key == hotkeyMoveUp && event.mods == hotkeyMoveMods)
				m_MovementPositive.y = 1;
			if (event.key == hotkeyMoveDown && event.mods == hotkeyMoveMods)
				m_MovementNegative.y = -1;
			if (event.key == hotkeyMoveRight && event.mods == hotkeyMoveMods)
				m_MovementPositive.x = 1;
			if (event.key == hotkeyMoveLeft && event.mods == hotkeyMoveMods)
				m_MovementNegative.x = -1;
		}
		if (event.type == EventType::KeyRelease) {
			if (event.key == hotkeyMoveForward)
				m_MovementPositive.z = 0;
			if (event.key == hotkeyMoveBackward)
				m_MovementNegative.z = 0;
			if (event.key == hotkeyMoveUp)
				m_MovementPositive.y = 0;
			if (event.key == hotkeyMoveDown)
				m_MovementNegative.y = 0;
			if (event.key == hotkeyMoveRight)
				m_MovementPositive.x = 0;
			if (event.key == hotkeyMoveLeft)
				m_MovementNegative.x = 0;
		}
		return false;
	}

	bool CameraController3D::OnMouseEvent(const ft::MouseEvent& event) {
		WorldCamera3D* camera = Renderer3D::GetCamera();
		if (event.type == EventType::MousePress) {
			MousePressEvent pressEvent = As<MousePressEvent>(event);
			if (pressEvent.button == GLFW_MOUSE_BUTTON_MIDDLE) {
				if (pressEvent.mods & hotkeyPanMods)
					m_Panning = true;
				else
					m_Rotating = true;
			}
		}
		if (event.type == EventType::MouseRelease) {
			MouseReleaseEvent releaseEvent = As<MouseReleaseEvent>(event);
			if (releaseEvent.button == GLFW_MOUSE_BUTTON_MIDDLE) {
				m_Panning = false;
				m_Rotating = false;
			}
		}
		if (event.type == EventType::MouseScroll) {
			MouseScrollEvent scrollEvent = As<MouseScrollEvent>(event);
			camera->Translate(camera->GetFront() * scrollEvent.yDelta * zoomSpeed * m_BaseZoomSpeed);
		}
		return false;
	}
}