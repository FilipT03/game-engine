#include "pch.h"
#include "Vector.h"

namespace ft {
	const glm::vec3 Vector::Up = glm::vec3(0.0f, 1.0f, 0.0f);
	const glm::vec3 Vector::Down = glm::vec3(0.0f, -1.0f, 0.0f);
	const glm::vec3 Vector::Left = glm::vec3(-1.0f, 0.0f, 0.0f);
	const glm::vec3 Vector::Right = glm::vec3(1.0f, 0.0f, 0.0f);
	const glm::vec3 Vector::Forward = glm::vec3(0.0f, 0.0f, -1.0f);
	const glm::vec3 Vector::Back = glm::vec3(0.0f, 0.0f, 1.0f);

	const glm::vec3 Vector::Zero = glm::vec3(0.0f);
	const glm::vec3 Vector::One = glm::vec3(1.0f);
}