#pragma once

#include "Core/Core.h"
#include "Core/Application.h"
#include "UI/UIModule.h"

namespace ft {

	class UI
	{
	public:
		template <typename ElementType, typename... Args>
		inline static UIElement* AddElement(Args&&... args) {
			return s_UIModule->AddElement<ElementType>(std::forward<Args>(args)...);
		}
		inline static void RemoveElement(uint32_t elementID) {
			s_UIModule->RemoveElement(elementID);
		}
	private:
		inline static UIModule* s_UIModule = nullptr;
		friend class Application;
	};
}

