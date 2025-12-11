#include "pch.h"
#include "UIElement.h"
#include "Core/Input.h"
#include "Renderer/Renderer2DInternal.h"
#include "API/Renderer2D.h"

namespace ft {
	bool UIElement::InBounds(float x, float y) const {
		float difX = abs(x - m_Rect.x);
		float difY = abs(y - m_Rect.y);
		return difX <= m_Rect.width  / 2
			&& difY <= m_Rect.height / 2;
	}

	Transform UIElement::GenerateTransform() const { // TODO: Add other pixel scalings
		Transform transform = Transform();
		transform.position = glm::vec2(m_Rect.x, m_Rect.y);
		transform.scale = glm::vec2(m_Rect.width, m_Rect.height);
		return transform;
	}

	void UIElement::SetRect(Rect rect) {
		m_Rect = rect;
		m_Shape->transform = GenerateTransform();
		m_Shape->UpdateWorldVertices();
	}

	Button::Button(const std::string& texturePath, Rect rect) : UIElement(rect), m_IsPressed(false), m_IsHovered(false) {
		m_Texture = AssetManager::LoadTexture(texturePath);
		SetTints();
	}

	bool Button::OnMouseEvent(const MouseEvent& e)
	{
		bool consumed = false;
		switch (e.type)
		{
		case EventType::MouseMove: 
		{
			auto& moveEvent = As<MouseMoveEvent>(e);
			m_IsHovered = InBounds(moveEvent.x, moveEvent.y);
			break;
		}
		case EventType::MousePress:
			glm::vec2 position = Input::GetMousePosition();
			m_IsPressed = InBounds(position.x, position.y);
			consumed = m_IsPressed;
			break;
		case EventType::MouseRelease:
			position = Input::GetMousePosition();
			bool inBounds = InBounds(position.x, position.y);
			if (m_IsPressed && inBounds) 
			{
				m_ClickCallback();
				consumed = true;
				break;
			}
			m_IsPressed = false;
			break;
		}
		UpdateColor();
		return consumed;
	}

	void Button::UpdateColor() const
	{
		if (m_IsPressed)
			m_Shape->color = m_PressTint;
		else if (m_IsHovered)
			m_Shape->color = m_HoverTint;
		else
			m_Shape->color = m_ActiveTint;
	}

	void Button::RegisterShape()
	{
		Transform transform = GenerateTransform();
		m_Shape = Renderer2D::AddUIShape<TextureQuad>(m_Texture, transform, m_ActiveTint);
	}

	void Button::RemoveShape()
	{
		Transform transform = GenerateTransform();
		Renderer2D::RemoveUIShape(m_Shape->GetID());
	}
}
