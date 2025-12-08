#pragma once

#include "Core/Core.h"

namespace ft {

	class RenderingContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		static RenderingContext* Create(void* windowHandle);
	};
}

