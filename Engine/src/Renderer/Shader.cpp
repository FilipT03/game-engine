#include "pch.h"
#include "Shader.h"
#include "Core/Log.h"
#ifdef FT_OPENGL_RENDERER
#include "Platform/OpenGL/GLShader.h"
#endif

namespace ft {

	Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc) {
		#ifdef FT_OPENGL_RENDERER
			return new GLShader(vertexSrc, fragmentSrc);
		#else
			FT_ENGINE_ERROR("Unknown rendering API");
			return nullptr;
		#endif
	}
}