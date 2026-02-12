#pragma once

#include "Core/Core.h"

#include<glm/glm.hpp>

namespace ft {

	class LightSource
	{
	public:
		glm::vec3 position;
		glm::vec3 color;
		float intensity;
		float ambientIntensity = 0.2f;

		LightSource(const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& color = glm::vec3(1.0f), float intensity = 1.0f)
			: position(position), color(color), intensity(intensity) {}
	};
}

