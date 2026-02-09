#include "pch.h"
#include "Mesh.h"
#include "Math/Vector.h"

namespace ft {
	Mesh::Mesh(
		const Transform3D& transform = Transform3D(),
		const glm::vec4& color = glm::vec4(1.0f),
		bool isStatic = false)
		: transform(transform), color(color), m_ID(0), m_Static(isStatic)
	{
		m_Data = std::make_unique<MeshData>();
		m_RenderMesh = std::make_unique<RenderMesh>(isStatic);
		CalculateModelMatrix();
		BakeToRenderMesh();
	}

	Mesh::Mesh(
		MeshData data,
		const Transform3D& transform = Transform3D(),
		const glm::vec4& color = glm::vec4(1.0f),
		bool isStatic = false)
		: transform(transform), color(color), m_ID(0), m_Static(isStatic)
	{
		m_Data = std::make_unique<MeshData>(std::move(data));
		m_RenderMesh = std::make_unique<RenderMesh>(isStatic);
		CalculateModelMatrix();
		BakeToRenderMesh();
	}

	void Mesh::BakeToRenderMesh()
	{
		RenderMesh* renderMesh = GetRenderMesh();
		renderMesh->vertices.clear();
		renderMesh->indices.clear();

		uint32_t v = 0; // global vertex index
		for (uint32_t f = 0; f < m_Data->polygonSizes.size(); f++) {
			uint32_t polygonSize = m_Data->polygonSizes[f];
			for (uint32_t i = 0; i < polygonSize; i++) {
				uint32_t vertexIndex = m_Data->indices[v + i];
				glm::vec3 normal;
				if (m_Data->IsSmoothingEnabled())
					normal = m_Data->cornerNormals[v + i];
				else
					normal = m_Data->faceNormals[f];
				renderMesh->vertices.push_back(Vertex(m_Data->positions[vertexIndex], normal));
			}
			if (m_RenderMode == RenderMode::Wireframe) {
				for (uint32_t i = 0; i < polygonSize; i++) {
					renderMesh->indices.push_back(v + i);
					renderMesh->indices.push_back(v + (i + 1) % polygonSize);
				}
			} 
			else {
				for (uint32_t i = 2; i < polygonSize; i++) {
					renderMesh->indices.push_back(v);
					renderMesh->indices.push_back(v + i - 1);
					renderMesh->indices.push_back(v + i);
				}
			}
			v += polygonSize;
		}
	}

	Mesh Mesh::CreateCube(const Transform3D& transform, const glm::vec4& color, bool isStatic)
	{	
		return Mesh(MeshData::CreateCube(), transform, color, isStatic);
	}
	Mesh Mesh::CreateSphere(const Transform3D& transform, const glm::vec4& color, uint32_t segmentCount, uint32_t ringCount, bool isStatic)
	{
		return Mesh(MeshData::CreateSphere(segmentCount, ringCount), transform, color, isStatic);
	}
	Mesh Mesh::CreateCylinder(const Transform3D& transform, const glm::vec4& color, uint32_t segmentCount, bool isStatic)
	{
		return Mesh(MeshData::CreateCylinder(segmentCount), transform, color, isStatic);
	}
	Mesh Mesh::CreateCone(const Transform3D& transform, const glm::vec4& color, uint32_t segmentCount, bool isStatic)
	{
		return Mesh(MeshData::CreateCone(segmentCount), transform, color, isStatic);
	}
	Mesh Mesh::CreatePlane(const Transform3D& transform, const glm::vec4& color, bool isStatic)
	{
		return Mesh(MeshData::CreatePlane(), transform, color, isStatic);
	}

	void Mesh::CalculateModelMatrix()
	{
		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, transform.position);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotation.x), Vector::Right);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotation.y), Vector::Up);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotation.z), Vector::Forward);
		modelMatrix = glm::scale(modelMatrix, transform.scale);
	}
}