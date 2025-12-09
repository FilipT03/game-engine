#pragma once

#include "Renderer/Renderer2DInternal.h"

namespace ft {
	class Renderer2D {
	public:
		inline static void Clear() { 
			s_Renderer->Clear(); 
		}
		inline static void SetClearColor(float r, float g, float b, float a) {
			s_Renderer->SetClearColor(r, g, b, a);
		}

		template <typename ShapeType, typename... Args>
		inline static Shape* AddShape(Args&&... args) {
			return s_Renderer->AddShape<ShapeType>(std::forward<Args>(args)...);
		}

		inline static Camera* GetCamera() {
			return s_Renderer->GetCamera();
		}
		inline static glm::vec2 ScreenToWorld(glm::vec2 screenCoordinates) {
			return s_Renderer->GetCamera()->ScreenToWorld(screenCoordinates);
		}
		inline static glm::vec2 ScreenDeltaToWorld(glm::vec2 screenDelta) {
			return s_Renderer->GetCamera()->ScreenDeltaToWorld(screenDelta);
		}
		inline static glm::vec2 WorldToScreen(glm::vec2 worldCoordinates) {
			return s_Renderer->GetCamera()->WorldToScreen(worldCoordinates);
		}

	private:
		inline static Renderer2DInternal* s_Renderer = nullptr;
		friend class Application;
	};
}