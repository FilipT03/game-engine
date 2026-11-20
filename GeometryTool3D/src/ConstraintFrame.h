#pragma once

#include<glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>
#include<Math/Vector.h>
#include<Renderer/3D/Transform3D.h>

enum class ConstraintMode {
	None,
	World,
	Local,
	Normal
};

enum class ConstraintAxis {
	X,
	Y,
	Z
};

struct ConstraintFrame
{
	glm::quat orientation;
	bool activeX = false, activeY = false, activeZ = false;
	ConstraintMode mode = ConstraintMode::None;

	bool CalculateFromYZ(glm::vec3 y, glm::vec3 z, ConstraintAxis primary) {
		y = glm::normalize(y);
		z = glm::normalize(z);
		bool valid = true;
		glm::vec3 x = GetCrossX(y, z, valid);
		if (!valid)
			return false;

		if (glm::abs(glm::dot(y, z)) >= 1e-6f) {
			if (primary == ConstraintAxis::Y)
				z = GetCrossZ(x, y, valid);
			if (primary == ConstraintAxis::Z)
				y = GetCrossY(x, z, valid);
			if (!valid)
				return false;
		}

		orientation = glm::quat_cast(glm::mat3(x, y, z));
		return true;
	}
	bool CalculateFromXY(glm::vec3 x, glm::vec3 y, ConstraintAxis primary) {
		x = glm::normalize(x);
		y = glm::normalize(y);
		bool valid = true;
		glm::vec3 z = GetCrossZ(x, y, valid);
		if (!valid)
			return false;

		if (glm::abs(glm::dot(x, y)) >= 1e-6f) {
			if (primary == ConstraintAxis::Y)
				x = GetCrossX(y, z, valid);
			if (primary == ConstraintAxis::X)
				y = GetCrossY(x, z, valid);
			if (!valid)
				return false;
		}

		orientation = glm::quat_cast(glm::mat3(x, y, z));
		return true;
	}
	bool CalculateFromXZ(glm::vec3 x, glm::vec3 z, ConstraintAxis primary) {
		x = glm::normalize(x);
		z = glm::normalize(z);
		bool valid = true;
		glm::vec3 y = GetCrossY(x, z, valid);
		if (!valid)
			return false;

		if (glm::abs(glm::dot(x, z)) >= 1e-6f) {
			if (primary == ConstraintAxis::X)
				z = GetCrossZ(x, y, valid);
			if (primary == ConstraintAxis::Z)
				x = GetCrossX(y, z, valid);
			if (!valid)
				return false;
		}

		orientation = glm::quat_cast(glm::mat3(x, y, z));
		return true;
	}
	void ClearActive() {
		activeX = activeY = activeZ = false;
	}
	void Clear() {
		activeX = activeY = activeZ = false;
		mode = ConstraintMode::None;
	}
	void SetCardinal() {
		orientation = glm::identity<glm::quat>();
	}
	void SetFromTransform(const ft::Transform3D& transform) {
		orientation = glm::quat_cast(transform.GetRotationMatrix());
	}
	void SetConstraint(ConstraintAxis axis) {
		if (axis == ConstraintAxis::X) activeX = true;
		if (axis == ConstraintAxis::Y) activeY = true;
		if (axis == ConstraintAxis::Z) activeZ = true;
	}
	// Accepts world, returns world
	glm::vec3 Apply(const glm::vec3& delta) {
		glm::vec3 consDelta = glm::inverse(orientation) * delta;
		if (!activeX) consDelta.x = 0;
		if (!activeY) consDelta.y = 0;
		if (!activeZ) consDelta.z = 0;
		return orientation * consDelta;
	}
	// Accepts world, returns constraint space
	glm::vec3 ProjectLocalConstrained(const glm::vec3& delta) const {
		glm::vec3 local(0.0f);
		if (activeX) local.x = glm::dot(delta, GetAxisX());
		if (activeY) local.y = glm::dot(delta, GetAxisY());
		if (activeZ) local.z = glm::dot(delta, GetAxisZ());
		return local;
	}
	// Accepts world, returns constraint space
	glm::vec3 ProjectLocal(const glm::vec3& delta) const {
		glm::vec3 local;
		local.x = glm::dot(delta, GetAxisX());
		local.y = glm::dot(delta, GetAxisY());
		local.z = glm::dot(delta, GetAxisZ());
		return local;
	}
	// Returns world space
	glm::vec3 GetConstrainedVector() const {
		glm::vec3 result{ 0.0f };
		if (activeX) result += GetAxisX();
		if (activeY) result += GetAxisY();
		if (activeZ) result += GetAxisZ();
		return result;
	}
	glm::vec3 GetUnconstrainedVector() const {
		glm::vec3 result{ 0.0f };
		if (!activeX) result += GetAxisX();
		if (!activeY) result += GetAxisY();
		if (!activeZ) result += GetAxisZ();
		return result;
	}
	glm::vec3 ActivityVector() const {
		return glm::vec3{
			activeX ? 1.0f : 0.0f,
			activeY ? 1.0f : 0.0f,
			activeZ ? 1.0f : 0.0f,
		};
	}
	glm::vec3 ToWorld(const glm::vec3& vector) const {
		return orientation * vector;
	}
	bool IsActive() const {
		return activeX || activeY || activeZ;
	}
	bool OneAxisActive() const {
		return activeX + activeY + activeZ == 1;
	}
	bool TwoAxesAcitve() const {
		return activeX + activeY + activeZ == 2;
	}
	bool CheckConstraint(ConstraintAxis value) const {
		if (activeX && value == ConstraintAxis::X)
			return true;
		if (activeY && value == ConstraintAxis::Y)
			return true;
		if (activeZ && value == ConstraintAxis::Z)
			return true;
		return false;
	}
	glm::vec3 GetAxisX() const {
		return orientation * ft::Vector::X;
	}
	glm::vec3 GetAxisY() const {
		return orientation * ft::Vector::Y;
	}
	glm::vec3 GetAxisZ() const {
		return orientation * ft::Vector::Z;
	}

private:
	glm::vec3 GetCrossX(const glm::vec3& y, const glm::vec3& z, bool& valid) {
		return ft::Vector::NormalizeOrDefault(glm::cross(y, z), ft::Vector::X, valid);
	}
	glm::vec3 GetCrossY(const glm::vec3& x, const glm::vec3& z, bool& valid) {
		return ft::Vector::NormalizeOrDefault(glm::cross(z, x), ft::Vector::Y, valid);
	}
	glm::vec3 GetCrossZ(const glm::vec3& x, const glm::vec3& y, bool& valid) {
		return ft::Vector::NormalizeOrDefault(glm::cross(x, y), ft::Vector::Z, valid);
	}
};