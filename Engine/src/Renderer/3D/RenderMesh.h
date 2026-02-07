#pragma once

#include "Core/Core.h"
#include "Mesh.h"

namespace ft {
	/// ===== Vertex =====
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		Vertex(const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& normal = glm::vec3(0.0f))
			: position(position), normal(normal) {}
	};

	/// ===== RenderMesh =====
	class RenderMesh
	{
	public:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		uint32_t vertexOffset = 0, vertexByteOffset = 0, indexOffset = 0;

		uint32_t GetVertexCount() const { return vertices.size(); }
		uint32_t GetIndexCount() const { return indices.size(); }
		uint32_t GetIndexByteSize() const { return sizeof(uint32_t) * indices.size(); }
	};
}

