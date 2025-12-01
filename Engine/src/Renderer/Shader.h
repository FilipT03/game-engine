#pragma once

#include "Core/Core.h"

#include <glad/glad.h>

namespace ft {

	class Shader
	{
	public:
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();

		void Bind() const;
		void Unbind() const;
	private:
		GLuint CompileShader(GLenum type, const std::string& src);
		uint32_t m_ID = 0;
	};
}

