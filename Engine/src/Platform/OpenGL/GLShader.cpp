#include "pch.h"
#include "GLShader.h"

namespace ft {
	/// Shader code taken and adapted from https://wikis.khronos.org/opengl/Shader_Compilation
	GLShader::GLShader(const std::string& vertexSrc, const std::string& fragmentSrc) {
		GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSrc);
		if (vertexShader == 0)
			return;

		GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);
		if (fragmentShader == 0)
		{
			glDeleteShader(vertexShader);
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		m_ID = glCreateProgram();
		GLuint program = m_ID;

		// Attach our shaders to our program
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			FT_ENGINE_ERROR("Shader linking failed: {}", infoLog.data());

			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(program, vertexShader);
		glDetachShader(program, fragmentShader);
	};

	GLShader::~GLShader() {
		glDeleteProgram(m_ID);
	};

	GLuint GLShader::CompileShader(GLenum type, const std::string& src)
	{
		// Create an empty shader handle
		GLuint shader = glCreateShader(type);

		// Send the shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* source = src.c_str();
		glShaderSource(shader, 1, &source, 0);

		// Compile the shader
		glCompileShader(shader);

		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(shader);

			FT_ENGINE_ERROR("{} shader compilation failed: {}", type == GL_FRAGMENT_SHADER ? "Fragment" : "Vertex", infoLog.data());

			return 0;
		}
		return shader;
	}

	void GLShader::Bind() const
	{
		glUseProgram(m_ID);
	}

	void GLShader::Unbind() const
	{
		glUseProgram(0);
	}
}