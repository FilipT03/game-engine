#include "pch.h"
#include "UIElement.h"
#include "Core/Input.h"
#include "Renderer/Renderer2DInternal.h"
#include "API/Renderer2D.h"

namespace ft {
	/// ============ UI Element ============
	bool UIElement::InBounds(float x, float y) const {
		return x >= m_Rect.x && y >= m_Rect.y
			&& x <= m_Rect.x + m_Rect.width
			&& y <= m_Rect.y + m_Rect.height;
	}

	Transform2D UIElement::GenerateTransform() const { // TODO: Add other pixel scalings
		Transform2D transform = Transform2D();
		transform.position = glm::vec2(m_Rect.x + m_Rect.width / 2.0, m_Rect.y + m_Rect.height / 2.0);
		transform.scale = glm::vec2(m_Rect.width, m_Rect.height);
		return transform;
	}

	void UIElement::SetRect(Rect rect) {
		m_Rect = rect;
		if (m_Shape == nullptr) return;
		m_Shape->transform = GenerateTransform();
		m_Shape->UpdateWorldVertices();
	}

	bool UIElement::OnMouseEvent(const MouseEvent& e)
	{
		if (e.type == EventType::MousePress)
			return InBounds(Input::GetMousePosition());
		return false;
	}

	/// ============ Button ============
	Button::Button(const std::string& texturePath, Rect rect) : UIElement(rect), m_IsPressed(false), m_IsHovered(false) {
		m_Texture = AssetManager::LoadTexture(texturePath);
		m_ClickCallback = [] {};
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
			m_IsPressed = InBounds(Input::GetMousePosition());
			consumed = m_IsPressed;
			break;
		case EventType::MouseRelease:
			bool inBounds = InBounds(Input::GetMousePosition());
			if (m_IsPressed && inBounds) 
			{
				m_ClickCallback();
				consumed = true;
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
		if (m_RegisteredShape)
			return;
		Transform2D transform = GenerateTransform();
		m_Shape = Renderer2D::AddUIShape<TextureQuad>(m_Texture, transform, m_ActiveTint);
		m_RegisteredShape = true;
	}

	void Button::SetTexture(const std::string& texturePath)
	{
		m_Texture = AssetManager::LoadTexture(texturePath);
		TextureQuad* quad = dynamic_cast<TextureQuad*>(m_Shape);
		quad->SetTexture(m_Texture);
	}

	/// ============ Panel ============
	Panel::Panel(Rect rect, const glm::vec4& color) : UIElement(rect), m_Color(color) {}

	Panel::Panel(const std::string& texturePath, Rect rect, const glm::vec4& color) : UIElement(rect), m_Color(color)
	{
		m_Texture = AssetManager::LoadTexture(texturePath);
	}

	void Panel::RegisterShape()
	{
		if (m_RegisteredShape)
			return;
		Transform2D transform = GenerateTransform();
		if (m_Texture == nullptr)
			m_Shape = Renderer2D::AddUIShape<Rectangle>(transform, m_Color);
		else
			m_Shape = Renderer2D::AddUIShape<TextureQuad>(m_Texture, transform, m_Color);
		m_RegisteredShape = true;
	}
}
