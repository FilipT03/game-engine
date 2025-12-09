#include "pch.h"
#include "Texture.h"
#ifdef FT_OPENGL_RENDERER
#include "Platform/OpenGL/GLTexture.h"
#endif

namespace ft {
	Texture* Texture::Create(const std::string& imagePath) {
		#ifdef FT_OPENGL_RENDERER
			return new GLTexture(imagePath);
		#else
			FT_ENGINE_ERROR("Unknown rendering API");
			return nullptr;
		#endif
	}
}