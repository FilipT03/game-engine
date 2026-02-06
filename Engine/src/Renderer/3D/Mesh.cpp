#include "pch.h"
#include "Mesh.h"

namespace ft {
	Mesh::Mesh(const Transform3D& transform = Transform3D()) : transform(transform), m_ID(0) {}

	Mesh Mesh::CreateCube(const Transform3D& transform)
	{
		Mesh mesh(transform);
		
		mesh.vertices = {};
		mesh.indices = {};
		for (int i = 0; i < 8; i++) {
			mesh.vertices.push_back({
				(i & 1) ? 0.5f : -0.5f,
				(i & 2) ? 0.5f : -0.5f,
				(i & 4) ? 0.5f : -0.5f });
		}

		for (int i = 0; i < 6; i++) {
			int offset = i * 4;
			mesh.indices.push_back(offset + 0);
			mesh.indices.push_back(offset + 1);
			mesh.indices.push_back(offset + 2);
			mesh.indices.push_back(offset + 0);
			mesh.indices.push_back(offset + 2);
			mesh.indices.push_back(offset + 3);
		}

		mesh.CalculateNormals();

		return mesh;
	}
	Mesh Mesh::CreateSphere(const Transform3D& transform, uint32_t segmentCount, uint32_t stackCount)
	{
		return Mesh();
	}
	Mesh Mesh::CreateCylinder(const Transform3D& transform, uint32_t segmentCount, float height)
	{
		return Mesh();
	}
	Mesh Mesh::CreateTetrahedron(const Transform3D& transform)
	{
		return Mesh();
	}
	Mesh Mesh::CreatePlane(const Transform3D& transform)
	{
		Mesh mesh(transform);

		mesh.vertices = {
			{-0.5f, 0.0f, -0.5f},
			{ 0.5f, 0.0f, -0.5f},
			{ 0.5f, 0.0f,  0.5f},
			{-0.5f, 0.0f,  0.5f}
		};
		mesh.indices = { 0, 1, 2, 0, 2, 3 };
		mesh.CalculateNormals();

		return mesh;
	}

	void Mesh::CalculateNormals()
	{
		normals = {};
		for (int i = 0; i < indices.size(); i += 3) {
			glm::vec3 a = vertices[indices[i + 1]] - vertices[indices[i]];
			glm::vec3 b = vertices[indices[i + 2]] - vertices[indices[i + 1]];
			glm::vec3 normal = glm::cross(a, b);
			normals.push_back(glm::normalize(normal));
		}
	}
}