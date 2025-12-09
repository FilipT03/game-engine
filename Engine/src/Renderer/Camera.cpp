#include "pch.h"
#include "Camera.h"
#include "Core/Application.h"

namespace ft {
	Camera::Camera() : m_Projection(0.0f), m_View(0.0f), m_ViewProjection(0.0f) {};

	glm::vec2 Camera::ScreenToWorld(glm::vec2 screenCoordinates) const
	{
		glm::vec2 windowSize = Application::Get().GetWindow().GetWindowSize();
		glm::vec2 ndc{};
		ndc.x = (screenCoordinates.x / windowSize.x) * 2.0f - 1.0f;
		ndc.y = 1.0f - (screenCoordinates.y / windowSize.y) * 2.0f;

		glm::mat4 invVP = glm::inverse(m_ViewProjection);
		glm::vec4 world = invVP * glm::vec4(ndc.x, ndc.y, 0.0f, 1.0f);
		return glm::vec2(world.x, world.y);
	}

	glm::vec2 Camera::ScreenDeltaToWorld(glm::vec2 screenDelta) const
	{
		glm::vec2 windowSize = Application::Get().GetWindow().GetWindowSize();

		float w = (m_Right - m_Left) / windowSize.x;
		float h = (m_Top - m_Bottom) / windowSize.y;

		return glm::vec2(screenDelta.x * w / m_Zoom, -screenDelta.y * h / m_Zoom);
	}

	glm::vec2 Camera::WorldToScreen(glm::vec2 worldCoordinates) const
	{
		glm::vec4 ndc = m_ViewProjection * glm::vec4(worldCoordinates.x, worldCoordinates.y, 0.0f, 1.0f);

		glm::vec2 windowSize = Application::Get().GetWindow().GetWindowSize();
		glm::vec2 screenCoords{};
		ndc = (ndc + 1.0f) / 2.0f; // transform to range [0,1]
		screenCoords.x = windowSize.x * ndc.x;
		screenCoords.y = windowSize.y * (1.0f - ndc.y);
		return screenCoords;
	}

	/// The smaller dimension will be Y units, and the larger will be Y x aspect ratio.
	void Camera::CalculateProjectionMatrix(float width, float height)
	{
		float aspect;
		if (width > height)
		{
			aspect = width / height;
			m_Left  = -aspect * 0.5f * FT_VIEW_UNITS;
			m_Right =  aspect * 0.5f * FT_VIEW_UNITS;
			m_Bottom = -0.5f * FT_VIEW_UNITS;
			m_Top    =  0.5f * FT_VIEW_UNITS;
		}
		else
		{
			aspect = height / width;
			m_Left  = -0.5f * FT_VIEW_UNITS;
			m_Right =  0.5f * FT_VIEW_UNITS;
			m_Bottom = -aspect * 0.5f * FT_VIEW_UNITS;
			m_Top    =  aspect * 0.5f * FT_VIEW_UNITS;
		}
		m_Projection = glm::ortho(m_Left, m_Right, m_Bottom, m_Top);

		m_ViewProjection = m_Projection * m_View;
	}

	void Camera::RecalculateView()
	{
		m_View = glm::scale(glm::mat4(1.0f), glm::vec3(m_Zoom, m_Zoom, 1.0f));
		m_View = glm::translate(m_View, glm::vec3(-m_Position, 0.0f));

		m_ViewProjection = m_Projection * m_View;
	}
}