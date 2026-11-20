#pragma once

#include "Core/Core.h"
#include "Math/Vector.h"

#include <glm/glm.hpp>

namespace ft {

	struct Transform3D {
		glm::vec3 position;
		glm::vec3 scale;
		glm::vec3 rotation;
		float zIndex;
		Transform3D(const glm::vec3& position = glm::vec3(0.0f),
			const glm::vec3& scale = glm::vec3(1.0f),
			const glm::vec3 rotation = glm::vec3(0.0f),
			float zIndex = 0.0f)
			: position(position), rotation(rotation), scale(scale), zIndex(zIndex) {}

		glm::mat4 GetRotationMatrix() const {
			glm::mat4 result(1.0f);
			result = glm::rotate(result, glm::radians(rotation.x), Vector::Right);
			result = glm::rotate(result, glm::radians(rotation.y), Vector::Up);
			result = glm::rotate(result, glm::radians(rotation.z), Vector::Forward);
			return result;
		}

		glm::mat4 GetMatrix() const {
			glm::mat4 result = glm::mat4(1.0f);
			result = glm::translate(result, position);
			result *= GetRotationMatrix();
			result = glm::scale(result, scale);
			return result;
		}

		glm::vec3 RotateVector(const glm::vec3& vector) const {
			return glm::vec3(GetRotationMatrix() * glm::vec4(vector, 0.0f));
		}
		glm::vec3 TransformVector(const glm::vec3& vector) const {
			return glm::vec3(GetMatrix() * glm::vec4(vector, 0.0f));
		}
		glm::vec3 InverseTransformVector(const glm::vec3& vector) const {
			return glm::vec3(glm::inverse(GetMatrix()) * glm::vec4(vector, 0.0f));
		}
		glm::vec3 TransformPoint(const glm::vec3& point) const {
			return glm::vec3(GetMatrix() * glm::vec4(point, 1.0f));
		}
		glm::vec3 InverseTransformPoint(const glm::vec3& point) const {
			return glm::vec3(glm::inverse(GetMatrix()) * glm::vec4(point, 1.0f));
		}

		glm::vec3 GetUp() const	     { return RotateVector(Vector::Up); }
		glm::vec3 GetDown() const    { return RotateVector(Vector::Down); }
		glm::vec3 GetRight() const   { return RotateVector(Vector::Right); }
		glm::vec3 GetLeft() const    { return RotateVector(Vector::Left); }
		glm::vec3 GetBack() const    { return RotateVector(Vector::Back); }
		glm::vec3 GetForward() const { return RotateVector(Vector::Forward); }
	};
}

