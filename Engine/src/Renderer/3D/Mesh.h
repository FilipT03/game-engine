#pragma once

#include "Core/Core.h"
#include "Core/Log.h"
#include "MeshData.h"
#include "RenderMesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace ft {

	/// ===== Transform3D =====
	struct Transform3D {
		glm::vec3 position;
		glm::vec3 scale;
		glm::vec3 rotation;
		float zIndex;
		Transform3D(const glm::vec3& position = glm::vec3(0.0f),
			const glm::vec3& scale = glm::vec3(1.0f),
			const glm::vec3 rotation = glm::vec3(0.0f),
			float zIndex = 0.0f)
			: position(position), rotation(rotation), scale(scale), zIndex(zIndex) {}
	};

	/// ===== Mesh =====
	class Mesh
	{
	public:
		Mesh(const Transform3D& transform, const glm::vec4& color);
		Mesh(MeshData data, const Transform3D& transform, const glm::vec4& color);

		Transform3D transform;
		glm::vec4 color;
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		
		uint32_t GetID() const { return m_ID; }
		MeshData* GetData() { 
			MarkDirty(); 
			return m_Data.get(); 
		}
		RenderMesh* GetRenderMesh() { return m_RenderMesh.get(); }

		void SetID(uint32_t id) {
			if (m_ID)
				FT_ENGINE_WARN("ID of mesh {} already set.", m_ID);
			else
				m_ID = id;
		}
		bool IsDirty() const { return m_Dirty; }
		void ResetDirty() { m_Dirty = false; }
		void MarkDirty() { m_Dirty = true; }

		void CalculateModelMatrix();
		void BakeToRenderMesh();

		static Mesh CreateCube(const Transform3D& transform = Transform3D());
		static Mesh CreateSphere(const Transform3D& transform = Transform3D(), uint32_t segmentCount = 36, uint32_t ringCount = 18);
		static Mesh CreateCylinder(const Transform3D& transform = Transform3D(), uint32_t segmentCount = 36);
		static Mesh CreateCone(const Transform3D& transform = Transform3D(), uint32_t segmentCount = 36);
		static Mesh CreatePlane(const Transform3D& transform = Transform3D());

	private:
		std::unique_ptr<MeshData> m_Data;
		std::unique_ptr<RenderMesh> m_RenderMesh;

		bool m_Dirty = false;
		uint32_t m_ID;
	};
}

