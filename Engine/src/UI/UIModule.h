#pragma once

#include "Core/Core.h"
#include "Core/Module.h"
#include "UI/UIElement.h"
#include "Renderer/Renderer2DInternal.h"

namespace ft {

	class UIModule : public Module
	{
	public:
		UIModule(Renderer2DInternal* UIRenderer) {}

		void OnUpdate() override;
		void OnRegister() override {}
		void OnClose() override {}
		void OnDelete() override {}

		bool OnMouseEvent(const MouseEvent& e) override;

		template <typename ElementType, typename... Args>
		UIElement* AddElement(Args&&... args)
		{
			static_assert(std::is_base_of<UIElement, ElementType>::value, "Element type must be derived from UIElement");
			auto element = std::make_unique<ElementType>(std::forward<Args>(args)...);
			element->SetID(m_LastElementID++);
			auto [it, success] = m_UIElements.insert({ element->GetID(), std::move(element) });
			return AddElementInternal(it->second.get());
		}
		void RemoveElement(uint32_t elementID);
	private:
		UIElement* AddElementInternal(UIElement* element);

		std::map<uint32_t, std::unique_ptr<UIElement>> m_UIElements;
		uint32_t m_LastElementID = 1;
	};
}

