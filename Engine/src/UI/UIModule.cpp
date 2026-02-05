#include "pch.h"
#include "UIModule.h"

namespace ft {
	UIElement* UIModule::AddElementInternal(UIElement* element)
	{
		element->RegisterShape();
		return element;
	}
	
	void UIModule::RemoveElement(uint32_t elementID)
	{
		if (!m_UIElements.contains(elementID))
			return;
		m_UIElements[elementID]->RemoveShape();
		m_UIElements.erase(elementID);
	}

	bool UIModule::OnMouseEvent(const MouseEvent& e)
	{
		for (auto it = m_UIElements.rbegin(); it != m_UIElements.rend(); ++it)
		{
			bool stop = it->second->OnMouseEvent(e);
			if (stop) return true;
		}
		return false;
	}

	void UIModule::OnUpdate()
	{
		for (auto& [id, element] : m_UIElements)
		{
			element->OnUpdate();
		}
	}
}