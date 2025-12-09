#pragma once

#include "Core/Core.h"

#include <glm/glm.hpp>

#define FT_VIEW_UNITS 100

namespace ft {

	class Camera
	{
	public:
		Camera();

		glm::vec2 ScreenToWorld(glm::vec2 screenCoordinates) const;
		glm::vec2 ScreenDeltaToWorld(glm::vec2 screenDelta) const;
		glm::vec2 WorldToScreen(glm::vec2 worldCoordinates) const;

		void CalculateProjectionMatrix(float width, float height);
		void RecalculateView();

		glm::mat4 GetViewProjection() const { return m_ViewProjection; }

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
		glm::vec2 m_Position = { 0.0f, 0.0f };
		float m_Zoom = 1.0f;

		float m_Left = 0, m_Right = 0, m_Top = 0, m_Bottom = 0;

		glm::mat4 m_View;
		glm::mat4 m_Projection;
		glm::mat4 m_ViewProjection;
	};
}

