#pragma once

#include "Core/Core.h"
#include "Renderer/Shader.h"

#include <glad/glad.h>

namespace ft {

	class GLShader : public Shader
	{
	public:
		GLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~GLShader();
		void Bind() const override;
		void Unbind() const override;

		void SetUniform1f(const std::string& name, float value) const override;
		void SetUniform3f(const std::string& name, const glm::vec3& value) const override;
		void SetUniform4f(const std::string& name, const glm::vec4& value) const override;
		void SetUniformMatrix4fv(const std::string& name, const glm::mat4 value, bool transpose = false) const override;
	private:
		GLuint CompileShader(GLenum type, const std::string& src);
		uint32_t m_ID = 0;
	};
}

