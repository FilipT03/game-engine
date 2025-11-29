#pragma once

#include "Core/Core.h"
#include "Event/InputEvent.h"

namespace ft {

	class FT_API Input
	{
	public:
		void Init(EventCallback eventCallback);
	private:
		EventCallback m_EventCallback;
	};
}

