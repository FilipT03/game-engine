#include "pch.h"
#include "GLTexture.h"
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace ft {
	GLTexture::GLTexture(const std::string& imagePath) 
	{
        LoadFromFile(imagePath);
        if (m_ID == 0)
            return;

        glBindTexture(GL_TEXTURE_2D, m_ID);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	};

	GLTexture::~GLTexture() 
	{
        glDeleteTextures(1, &m_ID);
	}

    void GLTexture::Bind(uint32_t unit) const
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, m_ID);
    };

	void GLTexture::LoadFromFile(const std::string& imagePath)
	{
        int width, height, channelCount;
        unsigned char* imageData = stbi_load(imagePath.c_str(), &width, &height, &channelCount, 0);
        if (imageData != NULL)
        {
            m_Width = width;
            m_Height = height;
            m_ChannelCount = channelCount;
            stbi__vertical_flip(imageData, m_Width, m_Height, m_ChannelCount);

            GLint internalFormat = -1;
            switch (m_ChannelCount) {
            case 1: internalFormat = GL_RED; break;
            case 2: internalFormat = GL_RG; break;
            case 3: internalFormat = GL_RGB; break;
            case 4: internalFormat = GL_RGBA; break;
            default: internalFormat = GL_RGB; break;
            }

            glGenTextures(1, &m_ID);
            glBindTexture(GL_TEXTURE_2D, m_ID);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, internalFormat, GL_UNSIGNED_BYTE, imageData);
            glBindTexture(GL_TEXTURE_2D, 0);

            stbi_image_free(imageData);
        }
        else
        {
            FT_ENGINE_ERROR("Failed to load texture from path: {}", imagePath);
            stbi_image_free(imageData);
        }
	}
}
