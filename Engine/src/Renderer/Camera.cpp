#include "pch.h"
#include "Camera.h"
#include "Core/Application.h"

namespace ft {
	/// ============ Camera 2D ============
	Camera::Camera() : m_Projection(0.0f), m_View(0.0f), m_ViewProjection(0.0f) {};


	/// ============ UI Camera ============
	void UICamera::CalculateProjectionMatrix(float width, float height)
	{
		m_Projection = glm::ortho(0.0f, width, height, 0.0f);
		m_ViewProjection = m_Projection;
	}

	void UICamera::RecalculateView()
	{
		m_View = glm::mat4(1.0f);
		m_ViewProjection = m_Projection;
	}


	/// ============ World Camera 2D ============
	glm::vec2 WorldCamera2D::ScreenToWorld(glm::vec2 screenCoordinates) const
	{
		glm::vec2 windowSize = Application::Get().GetWindow().GetWindowSize();
		glm::vec2 ndc{};
		ndc.x = (screenCoordinates.x / windowSize.x) * 2.0f - 1.0f;
		ndc.y = 1.0f - (screenCoordinates.y / windowSize.y) * 2.0f;

		glm::mat4 invVP = glm::inverse(m_ViewProjection);
		glm::vec4 world = invVP * glm::vec4(ndc.x, ndc.y, 0.0f, 1.0f);
		return glm::vec2(world.x, world.y);
	}

	glm::vec2 WorldCamera2D::ScreenDeltaToWorld(glm::vec2 screenDelta) const
	{
		glm::vec2 windowSize = Application::Get().GetWindow().GetWindowSize();

		float w = (m_Right - m_Left) / windowSize.x;
		float h = (m_Top - m_Bottom) / windowSize.y;

		return glm::vec2(screenDelta.x * w / m_Zoom, -screenDelta.y * h / m_Zoom);
	}

	glm::vec2 WorldCamera2D::WorldToScreen(glm::vec2 worldCoordinates) const
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
	void WorldCamera2D::CalculateProjectionMatrix(float width, float height)
	{
		float aspect;
		if (width > height)
		{
			aspect = width / height;
			m_Left = -aspect * 0.5f * FT_VIEW_UNITS;
			m_Right = aspect * 0.5f * FT_VIEW_UNITS;
			m_Bottom = -0.5f * FT_VIEW_UNITS;
			m_Top = 0.5f * FT_VIEW_UNITS;
		}
		else
		{
			aspect = height / width;
			m_Left = -0.5f * FT_VIEW_UNITS;
			m_Right = 0.5f * FT_VIEW_UNITS;
			m_Bottom = -aspect * 0.5f * FT_VIEW_UNITS;
			m_Top = aspect * 0.5f * FT_VIEW_UNITS;
		}
		m_Projection = glm::ortho(m_Left, m_Right, m_Bottom, m_Top);

		m_ViewProjection = m_Projection * m_View;
	}

	void WorldCamera2D::RecalculateView()
	{
		m_View = glm::scale(glm::mat4(1.0f), glm::vec3(m_Zoom, m_Zoom, 1.0f));
		m_View = glm::translate(m_View, glm::vec3(-m_Position, 0.0f));

		m_ViewProjection = m_Projection * m_View;
	}


	/// ============ World Camera 3D ============
	glm::vec3 WorldCamera3D::ScreenToWorld(glm::vec2 screenCoordinates, float depth) const
	{
		glm::vec3 rayOrigin, rayDir;
		ScreenPointToRay(screenCoordinates, rayOrigin, rayDir);
		glm::vec3 planeNormal = m_Front;
		float dot = glm::dot(rayDir, planeNormal);
		if (abs(dot) < 1e-6f)
			return rayOrigin + rayDir * depth;

		float distanceToPlane = depth - glm::dot(rayOrigin - m_Position, planeNormal);
		float t = distanceToPlane / dot;

		return rayOrigin + rayDir * t;
	}

	glm::vec3 WorldCamera3D::ScreenDeltaToWorld(glm::vec2 screenPos, glm::vec2 screenDelta, float depth) const
	{
		glm::vec3 p1 = ScreenToWorld(screenPos - screenDelta, depth);
		glm::vec3 p2 = ScreenToWorld(screenPos, depth);
		return p2 - p1;
	}

	glm::vec2 WorldCamera3D::WorldToScreen(glm::vec3 worldCoordinates) const
	{
		glm::vec4 ndc = m_ViewProjection * glm::vec4(worldCoordinates.x, worldCoordinates.y, 0.0f, 1.0f);

		glm::vec2 windowSize = Application::Get().GetWindow().GetWindowSize();
		glm::vec2 screenCoords{};
		ndc = (ndc + 1.0f) / 2.0f; // transform to range [0,1]
		screenCoords.x = windowSize.x * ndc.x;
		screenCoords.y = windowSize.y * (1.0f - ndc.y);
		return screenCoords;
	}

	void WorldCamera3D::ScreenPointToRay(glm::vec2 screenCoordinates, glm::vec3& rayOrigin, glm::vec3& rayDirection) const
	{
		glm::vec2 windowSize = Application::Get().GetWindow().GetWindowSize();
		glm::vec2 ndc{};
		ndc.x = (screenCoordinates.x / windowSize.x) * 2.0f - 1.0f;
		ndc.y = 1.0f - (screenCoordinates.y / windowSize.y) * 2.0f;

		glm::mat4 invVP = glm::inverse(m_ViewProjection);
		glm::vec4 worldNear = invVP * glm::vec4(ndc.x, ndc.y, -1.0f, 1.0f);
		glm::vec4 worldFar = invVP * glm::vec4(ndc.x, ndc.y, 1.0f, 1.0f);

		glm::vec3 nearPos = glm::vec3(worldNear) / worldNear.w;
		glm::vec3 farPos = glm::vec3(worldFar) / worldFar.w;

		rayOrigin = nearPos;
		rayDirection = glm::normalize(farPos - nearPos);
	}

	void WorldCamera3D::CalculateProjectionMatrix(float width, float height)
	{
		if (width == 0 || height == 0)
			return;

		m_Width = width;
		m_Height = height;
		if (m_ProjectionMode == ProjectionMode::Perspective)
		{
			float aspect = width / height;
			m_Projection = glm::perspective(glm::radians(m_Fov), aspect, m_Near, m_Far);
		}
		else
		{
			// Same as WorldCamera2D
			float zoom = 0.1f / tan(glm::radians(m_Fov) * 0.5f);
			float aspect;
			float units = FT_VIEW_UNITS * zoom;
			if (width > height)
			{
				aspect = width / height;
				m_Left = -aspect * 0.5f * units;
				m_Right = aspect * 0.5f * units;
				m_Bottom = -0.5f * units;
				m_Top = 0.5f * units;
			}
			else
			{
				aspect = height / width;
				m_Left = -0.5f * units;
				m_Right = 0.5f * units;
				m_Bottom = -aspect * 0.5f * units;
				m_Top = aspect * 0.5f * units;
			}
			m_Projection = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, m_Near, m_Far);
		}

		m_ViewProjection = m_Projection * m_View;
	}

	void WorldCamera3D::RecalculateView()
	{
		glm::vec3 worldUp = { 0.0f, 1.0f, 0.0f };
		if (m_Front.y > 0.99999f || m_Front.y < -0.99999f) // If looking straight up or down, use a different up vector to avoid gimbal lock
			worldUp = { 0.0f, 0.0f, -1.0f };
		m_View = glm::lookAt(m_Position, m_Position + m_Front, worldUp);

		m_ViewProjection = m_Projection * m_View;
	}
}