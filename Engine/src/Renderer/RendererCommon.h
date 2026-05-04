#pragma once

#include "Core/Core.h"
#include "BufferLayout.h"
#include <vector>

namespace ft {

	class RendererCommon
	{
	public:
		~RendererCommon() = default;

		static void Clear();
		static void SetClearColor(float r, float g, float b, float a);
		static void PackInterleaved(std::vector<uint8_t>& out, uint32_t vertexCount, const std::vector<const void*>& sources, const BufferLayout& layout);
	private:
		RendererCommon() = default;
	};
}

