#pragma once

#include "Renderer/3D/Renderer3DInternal.h"
#include "Renderer/RendererCommon.h"

namespace ft {

	class Renderer3D
	{
	public:
		inline static void Clear() {
			RendererCommon::Clear();
		}
		inline static void SetClearColor(float r, float g, float b, float a) {
			RendererCommon::SetClearColor(r, g, b, a);
		}
	private:
		Renderer3D() = default;
	};
}

