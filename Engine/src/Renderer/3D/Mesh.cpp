#include "pch.h"
#include "Mesh.h"
#include "Math/Vector.h"

namespace ft {
	Mesh::Mesh(
		const Transform3D& transform = Transform3D(),
		const glm::vec4& color = glm::vec4(1.0f)) 
		: transform(transform), color(color), m_ID(0) 
	{
		m_Data = std::make_unique<MeshData>();
		m_RenderMesh = std::make_unique<RenderMesh>();
		CalculateModelMatrix();
		BakeToRenderMesh();
	}

	Mesh::Mesh(
		MeshData data,
		const Transform3D& transform = Transform3D(),
		const glm::vec4& color = glm::vec4(1.0f))
		: transform(transform), color(color), m_ID(0)
	{
		m_Data = std::make_unique<MeshData>(std::move(data));
		m_RenderMesh = std::make_unique<RenderMesh>();
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
				renderMesh->vertices.push_back(Vertex(m_Data->positions[vertexIndex], m_Data->faceNormals[f]));
			}
			for (uint32_t i = 2; i < polygonSize; i++) {
				renderMesh->indices.push_back(v);
				renderMesh->indices.push_back(v + i - 1);
				renderMesh->indices.push_back(v + i);
			}
			v += polygonSize;
		}
	}

	Mesh Mesh::CreateCube(const Transform3D& transform)
	{	
		return Mesh(MeshData::CreateCube(), transform);
	}
	Mesh Mesh::CreateSphere(const Transform3D& transform, uint32_t segmentCount, uint32_t stackCount)
	{
		return Mesh(transform);
	}
	Mesh Mesh::CreateCylinder(const Transform3D& transform, uint32_t segmentCount, float height)
	{
		return Mesh(transform);
	}
	Mesh Mesh::CreateTetrahedron(const Transform3D& transform)
	{
		return Mesh(transform);
	}
	Mesh Mesh::CreatePlane(const Transform3D& transform)
	{
		return Mesh(MeshData::CreatePlane(), transform);
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