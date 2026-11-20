#pragma once

#include "Core/Core.h"

#include<glm/glm.hpp>

namespace ft {

	class Vector
	{
	public:
		~Vector() = default;

		static const glm::vec3 X, Y, Z;
		static const glm::vec3 Zero, One;
		static const glm::vec3 Up, Down, Left, Right, Forward, Back;

		// Returns default if vector is zero, NaN or Inf, otherwise normalized value.
		static glm::vec3 NormalizeOrDefault(const glm::vec3& vector, const glm::vec3& defaultValue);
		// Returns default if vector is zero, NaN or Inf, otherwise normalized value. Flag "normalized" is set if vector was normalized.
		static glm::vec3 NormalizeOrDefault(const glm::vec3& vector, const glm::vec3& defaultValue, bool& normalized);
		// Returns true if vector was normalized. Normalized vector is placed into the "normalizedVector" field.
		static bool TryNormalizeOrDefault(const glm::vec3& vector, const glm::vec3& defaultValue, glm::vec3& normalizedVector);

		static float Angle(const glm::vec3& vector1, const glm::vec3& vector2);

	private:
		Vector() = default;
	};
}

