#pragma once

#include "Core/Core.h"
#include "Event/Event.h"

#include "Renderer/Camera.h"
#include "Renderer/Buffers.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"
#include "Renderer/3D/LightSource.h"
#include "Mesh.h"

#include <set>

namespace ft {
	class Renderer3DInternal
	{
	public:
		Renderer3DInternal();
		~Renderer3DInternal();

		void Init();
		void Shutdown();

		Mesh* AddMesh(Mesh&& mesh);
		void RemoveMesh(uint32_t meshID);
		WorldCamera3D* GetCamera() { return m_Camera.get(); };
		LightSource* GetLightSource() { return m_LightSource.get(); };
		void SetightingMode(bool useStudioLighting) { m_UseStudioLighting = useStudioLighting; }

		void OnUpdate();
		bool OnEvent(const Event& event);
	private:
		struct CompareMeshPtr {
			bool operator()(const std::unique_ptr<Mesh>& lhs, const std::unique_ptr<Mesh>& rhs) const {
				return *lhs < *rhs;
			}
		};
		bool m_UseStudioLighting = true;

		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<VertexArray> m_VertexArray;
		std::set<std::unique_ptr<Mesh>, CompareMeshPtr> m_Meshes;
		uint32_t m_LastMeshId = 1;
		std::unique_ptr<Mesh> m_ModelMatrixCalculationMesh; // Used for calculating model matrices for stencil rendering

		uint32_t m_LastVertexByteOffset = 0, m_LastVertexVertexOffset = 0, m_LastIndexOffset = 0;
		std::unique_ptr<Shader> m_LitShader, m_UnlitShader, m_StudioShader;

		ProjectionMode m_PerspectiveMode = ProjectionMode::Perspective;
		std::unique_ptr<WorldCamera3D> m_Camera;
		std::unique_ptr<LightSource> m_LightSource;
	};
}

