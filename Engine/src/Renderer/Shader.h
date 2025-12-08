#pragma once

#include "Core/Core.h"
#include <glm/glm.hpp>

namespace ft {

	class Shader
	{
	public:
		Shader() = default;
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetUniform1f(const std::string& name, float value) const = 0;
		virtual void SetUniform3f(const std::string& name, const glm::vec3& value) const = 0;
		virtual void SetUniform4f(const std::string& name, const glm::vec4& value) const = 0;
		virtual void SetUniformMatrix4fv(const std::string& name, const glm::mat4 value, bool transpose = false) const = 0;

		static Shader* Create(const std::string& vertexSrc, const std::string& fragmentSrc);
	private:
	};
}

