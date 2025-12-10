#pragma once

#include "Renderer/Renderer2DInternal.h"

namespace ft {
	class Renderer2D {
	public:
		inline static void Clear() { 
			Renderer2DInternal::Clear();
		}
		inline static void SetClearColor(float r, float g, float b, float a) {
			Renderer2DInternal::SetClearColor(r, g, b, a);
		}

		template <typename ShapeType, typename... Args>
		inline static Shape* AddShape(Args&&... args) {
			return s_WorldRenderer->AddShape<ShapeType>(std::forward<Args>(args)...);
		}
		inline static void RemoveShape(uint32_t shapeID) {
			s_WorldRenderer->RemoveShape(shapeID);
		}

		template <typename ShapeType, typename... Args>
		inline static Shape* AddUIShape(Args&&... args) {
			return s_UIRenderer->AddShape<ShapeType>(std::forward<Args>(args)...);
		}
		inline static void RemoveUIShape(uint32_t shapeID) {
			s_UIRenderer->RemoveShape(shapeID);
		}

		inline static WorldCamera2D* GetCamera() {
			WorldCamera2D* worldCamera = dynamic_cast<WorldCamera2D*>(s_WorldRenderer->GetCamera());
			FT_ASSERT(worldCamera, "Renderer2D expects s_worldRenderer to have a 2D world camera");
			return worldCamera;
		}
		inline static glm::vec2 ScreenToWorld(glm::vec2 screenCoordinates) {
			return GetCamera()->ScreenToWorld(screenCoordinates);
		}
		inline static glm::vec2 ScreenDeltaToWorld(glm::vec2 screenDelta) {
			return GetCamera()->ScreenDeltaToWorld(screenDelta);
		}
		inline static glm::vec2 WorldToScreen(glm::vec2 worldCoordinates) {
			return  GetCamera()->WorldToScreen(worldCoordinates);
		}

	private:
		inline static Renderer2DInternal* s_WorldRenderer = nullptr;
		inline static Renderer2DInternal* s_UIRenderer = nullptr;
		friend class Application;
	};
}