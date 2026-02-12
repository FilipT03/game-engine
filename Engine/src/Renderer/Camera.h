#pragma once

#include "Core/Core.h"
#include "Math/Vector.h"

#include <glm/glm.hpp>

#define FT_VIEW_UNITS 100

namespace ft {
	enum class ProjectionMode {
		Perspective,
		Orthographic
	};

	class Camera
	{
	public:
		Camera();

		virtual void CalculateProjectionMatrix(float width, float height) = 0;
		virtual void RecalculateView() = 0;

		glm::mat4 GetViewProjection() const { return m_ViewProjection; }
		glm::mat4 GetProjection() const { return m_Projection; }
		glm::mat4 GetView() const { return m_View; }
		
	protected:
		glm::mat4 m_View;
		glm::mat4 m_Projection;
		glm::mat4 m_ViewProjection;
	};

	class UICamera : public Camera
	{
	public:
		void CalculateProjectionMatrix(float width, float height);
		void RecalculateView();
	};

	class WorldCamera2D : public Camera
	{
	public:
		void CalculateProjectionMatrix(float width, float height);
		void RecalculateView();

		glm::vec2 ScreenToWorld(glm::vec2 screenCoordinates) const;
		glm::vec2 ScreenDeltaToWorld(glm::vec2 screenDelta) const;
		glm::vec2 WorldToScreen(glm::vec2 worldCoordinates) const;

		glm::vec2 GetPosition() const { return m_Position; }
		float GetZoom() const { return m_Zoom; }

		void SetPosition(const glm::vec2& position)
		{
			m_Position = position;
			RecalculateView();
		}
		void SetZoom(float zoom)
		{
			m_Zoom = zoom;
			RecalculateView();
		}
		void Translate(const glm::vec2& deltaPosition)
		{
			m_Position += deltaPosition;
			RecalculateView();
		}
	private:
		float m_Left = 0, m_Right = 0, m_Top = 0, m_Bottom = 0;

		glm::vec2 m_Position = { 0.0f, 0.0f };
		float m_Zoom = 1.0f;
	};

	class WorldCamera3D : public Camera
	{
	public:
		void CalculateProjectionMatrix(float width, float height);
		void RecalculateView();

		glm::vec3 ScreenToWorld(glm::vec2 screenCoordinates, float depth) const;
		glm::vec3 ScreenDeltaToWorld(glm::vec2 screenPos, glm::vec2 screenDelta, float depth) const;
		glm::vec2 WorldToScreen(glm::vec3 worldCoordinates) const;
		void ScreenPointToRay(glm::vec2 screenCoordinates, glm::vec3& rayOrigin, glm::vec3& rayDirection) const;

		glm::vec3 GetPosition() const { return m_Position; }
		glm::vec3 GetFront() const { return m_Front; }
		glm::vec3 GetUp() const { return m_CameraUp; }
		glm::vec3 GetRight() const { return m_CameraRight; }
		float GetFov() const { return m_Fov; }

		void SetFov(float fov) {
			m_Fov = fov;
			CalculateProjectionMatrix(m_Width, m_Height);
		}
		void SetPosition(const glm::vec3& position) {
			m_Position = position;
			RecalculateView();
		}
		void Translate(const glm::vec3& deltaPosition) {
			m_Position += deltaPosition;
			RecalculateView();
		}
		void SetFront(const glm::vec3& front) {
			m_Front = glm::normalize(front);
			m_CameraRight = glm::normalize(glm::cross(m_Front, m_WorldUp));
			m_CameraUp = glm::normalize(glm::cross(m_CameraRight, m_Front));
			RecalculateView();
		}
		void SetWorldUp(const glm::vec3& up) {
			m_WorldUp = glm::normalize(up);
			m_CameraRight = glm::normalize(glm::cross(m_Front, m_WorldUp));
			m_CameraUp = glm::normalize(glm::cross(m_CameraRight, m_Front));
			RecalculateView();
		}

		ProjectionMode GetProjectionMode() const { return m_ProjectionMode; }
		void SetProjectionMode(ProjectionMode mode) {
			m_ProjectionMode = mode;
			RecalculateView();
			CalculateProjectionMatrix(m_Width, m_Height); // Use last width and height
		}


	private:
		float m_Left = 0, m_Right = 0, m_Top = 0, m_Bottom = 0;
		float m_Width = 0, m_Height = 0;
		float m_Near = 0.1f, m_Far = 100.0f;

		glm::vec3 m_Position = Vector::Zero;
		glm::vec3 m_Front = Vector::Forward;
		glm::vec3 m_WorldUp = Vector::Up;
		glm::vec3 m_CameraRight = Vector::Right;
		glm::vec3 m_CameraUp = Vector::Up;
		float m_Fov = 90.0f;

		ProjectionMode m_ProjectionMode = ProjectionMode::Perspective;
	};
}

