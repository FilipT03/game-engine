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

		inline static Camera2D* GetCamera() {
			return s_Renderer->GetCamera();
		}
		inline static void RecalculateView() {
			s_Renderer->RecalculateView();
		}
		inline static glm::vec2 ScreenToWorld(glm::vec2 screenCoordinates) {
			return s_Renderer->ScreenToWorld(screenCoordinates);
		}
		inline static glm::vec2 WorldToScreen(glm::vec2 worldCoordinates) {
			return s_Renderer->WorldToScreen(worldCoordinates);
		}

	private:
		inline static Renderer2DInternal* s_Renderer = nullptr;
		friend class Application;
	};
}