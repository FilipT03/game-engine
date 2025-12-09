#pragma once

#include "Core/Core.h"

namespace ft {

	class Texture
	{
	public:
		Texture() = default;
		virtual ~Texture() = default;

		virtual void Bind(uint32_t unit) const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetChannelCount() const = 0;

		static Texture* Create(const std::string& imagePath);
	};
}

