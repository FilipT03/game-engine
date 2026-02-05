#pragma once

#include "Core/Core.h"
#include "Renderer/Texture.h"

namespace ft {

	class GLTexture : public Texture
	{
	public:
		GLTexture(const std::string& imagePath);
		~GLTexture();

		void Bind(uint32_t unit) const;

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		uint32_t GetChannelCount() const { return m_ChannelCount; }
	private:
		void LoadFromFile(const std::string& imagePath);
		uint32_t m_Width = 0, m_Height = 0, m_ChannelCount = 0;
		uint32_t m_ID;
	};
}

