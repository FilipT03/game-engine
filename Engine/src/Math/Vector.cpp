#include "pch.h"
#include "Vector.h"

#include <glm/gtx/compatibility.hpp>

namespace ft {
	const glm::vec3 Vector::X = glm::vec3(1.0f, 0.0f, 0.0f);
	const glm::vec3 Vector::Y = glm::vec3(0.0f, 1.0f, 0.0f);
	const glm::vec3 Vector::Z = glm::vec3(0.0f, 0.0f, 1.0f);

	const glm::vec3 Vector::Zero = glm::vec3(0.0f);
	const glm::vec3 Vector::One = glm::vec3(1.0f);

	const glm::vec3 Vector::Up      =  Vector::Y;
	const glm::vec3 Vector::Down    = -Vector::Y;
	const glm::vec3 Vector::Right   =  Vector::X;
	const glm::vec3 Vector::Left    = -Vector::X;
	const glm::vec3 Vector::Back    =  Vector::Z;
	const glm::vec3 Vector::Forward = -Vector::Z;



	glm::vec3 Vector::NormalizeOrDefault(const glm::vec3& vector, const glm::vec3& defaultValue) {
		bool dummy;
		return NormalizeOrDefault(vector, defaultValue, dummy);
	}

	glm::vec3 Vector::NormalizeOrDefault(const glm::vec3& vector, const glm::vec3& defaultValue, bool& normalized) {
		float lengthSq = glm::dot(vector, vector);
		if (lengthSq < 1e-12f) {
			normalized = false;
			return defaultValue;
		}
		glm::vec3 normalizedVector = glm::normalize(vector);
		if (!glm::all(glm::isfinite(normalizedVector))) {
			normalized = false;
			return defaultValue;
		}
		normalized = true;
		return normalizedVector;
	}

	bool Vector::TryNormalizeOrDefault(const glm::vec3& vector, const glm::vec3& defaultValue, glm::vec3& normalizedVector) {
		bool normalized;
		normalizedVector = NormalizeOrDefault(vector, defaultValue, normalized);
		return normalized;
	}

	float Vector::Angle(const glm::vec3& vector1, const glm::vec3& vector2) {
		if (glm::dot(vector1, vector1) < 1e-12f || glm::dot(vector2, vector2) < 1e-12f)
			return 0.0f;
		float cosine = glm::dot(glm::normalize(vector1), glm::normalize(vector2));
		cosine = glm::clamp(cosine, -1.0f, 1.0f);
		return glm::degrees(glm::acos(cosine));
	}
}