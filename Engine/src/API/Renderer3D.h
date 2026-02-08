#pragma once

#include "Renderer/3D/Renderer3DInternal.h"
#include "Renderer/RendererCommon.h"

namespace ft {

	class Renderer3D
	{
	public:
		inline static void Clear() {
			RendererCommon::Clear();
		}
		inline static void SetClearColor(float r, float g, float b, float a) {
			RendererCommon::SetClearColor(r, g, b, a);
		}

		inline static Mesh* AddMesh(Mesh&& mesh) {
			return s_Renderer->AddMesh(std::move(mesh));
		}
		inline static void RemoveMesh(uint32_t meshID) {
			s_Renderer->RemoveMesh(meshID);
		}

		inline static WorldCamera3D* GetCamera() {
			return s_Renderer->GetCamera();
		}
		inline static LightSource* GetLightSource() {
			return s_Renderer->GetLightSource();
		}
	private:
		Renderer3D() = default;
		inline static Renderer3DInternal* s_Renderer = nullptr;
		friend class Application;
	};
}

