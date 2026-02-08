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

	void MeshData::CalculateNormals()
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

		data.CalculateNormals();
		return data;
	}

	MeshData MeshData::CreateSphere(uint32_t segmentCount, uint32_t ringCount)
	{
		if (segmentCount < 3) segmentCount = 3;
		if (ringCount < 3) ringCount = 3;

		MeshData data;
		float radius = 0.5f;
		data.positions = { {0.0f, radius, 0.0f} }; // Top vertex
		for (int ring = 1; ring < ringCount; ring++)
		{
			float phi = (float)ring / ringCount * glm::pi<float>(); // angle from the Y axis [0, pi]
			for (int segment = 0; segment < segmentCount; segment++) 
			{
				float theta = (float)segment / segmentCount * glm::two_pi<float>(); // angle around the Y axis [0, 2pi]
				data.positions.push_back({
					radius * glm::sin(phi) * glm::cos(theta),
					radius * glm::cos(phi),
					radius * glm::sin(phi) * glm::sin(theta)
				});
			}
		}
		data.positions.push_back({ 0.0f, -radius, 0.0f }); // Bottom vertex
		
		// Top cap
		for (int segment = 0; segment < segmentCount; segment++) {
			data.indices.push_back(0);
			data.indices.push_back(1 + (segment + 1) % segmentCount);
			data.indices.push_back(1 + segment);
			data.polygonSizes.push_back(3);
		}
		// Middle rings
		for (int ring = 0; ring < ringCount - 2; ring++) {
			for (int segment = 0; segment < segmentCount; segment++) {
				// 1 0
				// 3 2
				int point0 = 1 + ring * segmentCount + segment;
				int point1 = 1 + ring * segmentCount + (segment + 1) % segmentCount;
				int point2 = 1 + (ring + 1) * segmentCount + segment;
				int point3 = 1 + (ring + 1) * segmentCount + (segment + 1) % segmentCount;

				data.indices.push_back(point0);
				data.indices.push_back(point1);
				data.indices.push_back(point3);
				data.indices.push_back(point2);
				data.polygonSizes.push_back(4);
			}
		}
		// Bottom cap
		int bottomIndex = (int)data.positions.size() - 1;
		for (int segment = 0; segment < segmentCount; segment++) {
			data.indices.push_back(bottomIndex);
			data.indices.push_back(1 + (ringCount - 2) * segmentCount + segment);
			data.indices.push_back(1 + (ringCount - 2) * segmentCount + (segment + 1) % segmentCount);
			data.polygonSizes.push_back(3);
		}

		data.CalculateNormals();
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

		data.CalculateNormals();
		return data;
	}
}