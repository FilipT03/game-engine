#include "pch.h"
#include "MeshData.h"

namespace ft {
	void MeshData::Clear()
	{
		positions.clear();
		indices.clear();
		polygonSizes.clear();
		faceNormals.clear();
	}

	void MeshData::CalculateNormals(bool smooth)
	{
		faceNormals.clear();
		// f - face index, v - vertex index
		size_t v = 0;
		for (size_t f = 0; f < polygonSizes.size(); f++) {
			glm::vec3 a = positions[indices[v + 1]] - positions[indices[v]];
			glm::vec3 b = positions[indices[v + 2]] - positions[indices[v + 1]];
			glm::vec3 normal = glm::cross(a, b);
			faceNormals.push_back(glm::normalize(normal));

			v += polygonSizes[f];
		}
	}

	MeshData MeshData::CreateCube()
	{
		MeshData data;
		for (int i = 0; i < 8; i++) {
			data.positions.push_back({
					(i & 1) ? 0.5f : -0.5f,
					(i & 2) ? 0.5f : -0.5f,
					(i & 4) ? 0.5f : -0.5f });
		}

		data.indices = {
			4, 5, 7, 6, // Front face  (z =  0.5)
			1, 0, 2, 3, // Back face   (z = -0.5)
			5, 1, 3, 7, // Right face  (x =  0.5)
			0, 4, 6, 2, // Left face   (x = -0.5)
			6, 7, 3, 2, // Top face    (y =  0.5)
			0, 1, 5, 4  // Bottom face (y = -0.5)
		};
		data.polygonSizes = { 4, 4, 4, 4, 4, 4 };

		data.CalculateNormals(false);
		return data;
	}

	MeshData MeshData::CreatePlane()
	{
		MeshData data;
		data.positions = {
			{-0.5f, 0.0f, -0.5f},
			{ 0.5f, 0.0f, -0.5f},
			{ 0.5f, 0.0f,  0.5f},
			{-0.5f, 0.0f,  0.5f}
		};
		data.indices = { 0, 1, 2, 3 };
		data.polygonSizes = { 4 };

		data.CalculateNormals(false);
		return data;
	}
}