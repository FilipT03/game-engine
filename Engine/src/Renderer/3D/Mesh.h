#pragma once

#include "Core/Core.h"
#include "Core/Log.h"

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

	class Mesh
	{
	public:
		Mesh(const Transform3D& transform);
		~Mesh() = default;

		Transform3D transform;

		std::vector<glm::vec3> vertices;
		std::vector<uint32_t> indices;
		std::vector<glm::vec3> normals;

		uint32_t vertexOffset = 0, vertexByteOffset = 0, indexOffset = 0;

		uint32_t GetVertexCount() const { return vertices.size(); }
		uint32_t GetIndexCount() const { return indices.size(); }
		uint32_t GetIndexByteSize() const { return sizeof(uint32_t) * indices.size(); }
		uint32_t GetID() const { return m_ID; }

		void SetID(uint32_t id) {
			if (m_ID)
				FT_ENGINE_WARN("ID of mesh {} already set.", m_ID);
			else
				m_ID = id;
		}

		void CalculateNormals();

		static Mesh CreateCube(const Transform3D& transform = Transform3D());
		static Mesh CreateSphere(const Transform3D& transform = Transform3D(), uint32_t segmentCount = 36, uint32_t stackCount = 18);
		static Mesh CreateCylinder(const Transform3D& transform = Transform3D(), uint32_t segmentCount = 36, float height = 1.0f);
		static Mesh CreateTetrahedron(const Transform3D& transform = Transform3D());
		static Mesh CreatePlane(const Transform3D& transform = Transform3D());

	private:
		bool m_Dirty = false;
		uint32_t m_ID;
	};
}

