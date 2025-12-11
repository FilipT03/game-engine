#pragma once

#include "Core/Core.h"
#include "Renderer/Texture.h"
#include "Renderer/Shape.h"
#include "Renderer/Renderer2DInternal.h"
#include "Event/InputEvent.h"

namespace ft {
	struct Rect {
		float x, y, width, height;
		Rect(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {};
	};

	typedef std::function<void(void)> ClickCallback;

	class UIElement
	{
	public:
		UIElement(Rect rect) : m_Rect(rect), m_ID(0), m_Shape(nullptr) {}
		virtual ~UIElement() = default;
		virtual bool OnMouseEvent(const MouseEvent& e) { return false; }
		virtual void OnUpdate() {}
		
		virtual void RegisterShape() {};
		virtual void RemoveShape() {};

		uint32_t GetID() const { return m_ID; }
		void SetID(uint32_t id) {
			if (m_ID)
				FT_ENGINE_WARN("ID of UIElement {} already set.", m_ID);
			else
				m_ID = id;
		}
		bool InBounds(float x, float y) const;
		const Rect& GetRect() const { return m_Rect; }
		void SetRect(Rect rect);
		
	protected:
		Transform GenerateTransform() const;

		Rect m_Rect;
		uint32_t m_ID;
		Shape* m_Shape;
	};

	class Button : public UIElement
	{
	public:
		Button(const std::string& texturePath, Rect rect);
		bool OnMouseEvent(const MouseEvent& e) override;
		void OnUpdate() override {}

		void SetClickCallback(ClickCallback callback) { m_ClickCallback = callback; }
		void RegisterShape() override;
		void RemoveShape() override;

		void SetTints(const glm::vec4& activeTint =  glm::vec4(1.0f),
					  const glm::vec4& pressTint =   glm::vec4(0.8f, 0.8f, 0.8f, 1.0f),
					  const glm::vec4& hoverTint =   glm::vec4(0.9f, 0.9f, 0.9f, 1.0f),
					  const glm::vec4& disableTint = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f)) {
			SetActiveTint(activeTint);
			SetPressTint(pressTint);
			SetHoverTint(hoverTint);
			SetDisableTint(disableTint);
		}
		void SetPressTint(const glm::vec4& tint)   { m_PressTint = tint; }
		void SetHoverTint(const glm::vec4& tint)   { m_HoverTint = tint; }
		void SetActiveTint(const glm::vec4& tint)  { m_ActiveTint = tint; }
		void SetDisableTint(const glm::vec4& tint) { m_DisableTint = tint; }

	private:
		void UpdateColor() const;
		std::shared_ptr<Texture> m_Texture;
		glm::vec4 m_PressTint, m_HoverTint, m_DisableTint, m_ActiveTint;

		ClickCallback m_ClickCallback;
		
		bool m_IsHovered, m_IsPressed;
	};
}

