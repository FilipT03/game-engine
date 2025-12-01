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
	private:
		GLuint CompileShader(GLenum type, const std::string& src);
		uint32_t m_ID = 0;
	};
}

