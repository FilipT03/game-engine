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

		if (m_SmoothingMode == SmoothingMode::Smooth || m_SmoothingMode == SmoothingMode::SmoothByAngle)
		{
			CalculateVertexToFaceMap();
			cornerNormals.clear();
			float modeThershold = (m_SmoothingMode == SmoothingMode::Smooth) ? 180.0f : m_SmoothingTheshold;
			float angleThreshold = glm::cos(glm::radians(modeThershold));
			size_t v = 0;
			for (size_t f = 0; f < polygonSizes.size(); f++) { // each faces

				for (size_t i = 0; i < polygonSizes[f]; i++) { // each vertices of the face
					glm::vec3 normalSum(0.0f);
					uint32_t vertexIndex = indices[v + i];

					for (uint32_t adjacentFaceIndex : vertexToFaceMap[vertexIndex]) { // each face adjacent to the vertex
						float angleCos = glm::dot(faceNormals[f], faceNormals[adjacentFaceIndex]);
						if (angleCos >= angleThreshold)
							normalSum += faceNormals[adjacentFaceIndex];
					}

					if (glm::length(normalSum) < 0.001f)
						normalSum = faceNormals[f];
					cornerNormals.push_back(glm::normalize(normalSum));
				}
				v += polygonSizes[f];
			}
		}
	}


	void MeshData::CalculateVertexToFaceMap()
	{
		vertexToFaceMap.clear();
		vertexToFaceMap.resize(positions.size());
		size_t v = 0;
		for (size_t f = 0; f < polygonSizes.size(); f++) {
			for (int i = 0; i < polygonSizes[f]; i++) {
				uint32_t vertexIndex = indices[v + i];
				vertexToFaceMap[vertexIndex].push_back(f);
			}
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
		if (ringCount < 2) ringCount = 2;

		MeshData data;
		float radius = 0.5f;
		data.positions = { {0.0f, radius, 0.0f} }; // Top vertex
		for (int ring = 1; ring < ringCount; ring++)
		{
			float phi = (float)ring / ringCount * glm::pi<float>(); // angle from the Y axis [0, pi]
			for (int segment = 0; segment < segmentCount; segment++) 
			{
				float theta = glm::two_pi<float>() * (float)segment / (float)segmentCount; // angle around the Y axis [0, 2pi]
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
			int next = (segment + 1) % segmentCount;
			data.indices.push_back(0);
			data.indices.push_back(1 + next);
			data.indices.push_back(1 + segment);
			data.polygonSizes.push_back(3);
		}
		// Middle rings
		for (int ring = 0; ring < ringCount - 2; ring++) {
			for (int segment = 0; segment < segmentCount; segment++) {
				// 1 0
				// 3 2
				int next = (segment + 1) % segmentCount;
				int point0 = 1 + ring * segmentCount + segment;
				int point1 = 1 + ring * segmentCount + next;
				int point2 = 1 + (ring + 1) * segmentCount + segment;
				int point3 = 1 + (ring + 1) * segmentCount + next;

				data.indices.push_back(point0);
				data.indices.push_back(point1);
				data.indices.push_back(point3);
				data.indices.push_back(point2);
				data.polygonSizes.push_back(4);
			}
		}
		// Bottom cap
		int bottomIndex = (int)data.positions.size() - 1;
		int offset = 1 + (ringCount - 2) * segmentCount; // index of the first vertex in the last ring
		for (int segment = 0; segment < segmentCount; segment++) {
			int next = (segment + 1) % segmentCount;
			data.indices.push_back(bottomIndex);
			data.indices.push_back(offset + segment);
			data.indices.push_back(offset + next);
			data.polygonSizes.push_back(3);
		}

		data.m_SmoothingMode = SmoothingMode::Smooth;
		data.CalculateNormals();
		return data;
	}


	MeshData MeshData::CreateCylinder(uint32_t segmentCount)
	{
		if (segmentCount < 3) segmentCount = 3;

		MeshData data;
		float radius = 0.5f;
		for (int i = 0; i < 2; i++)
		{
			float y = (i == 0) ? 0.5f : -0.5f;
			for (int segment = 0; segment < segmentCount; segment++)
			{
				float theta = glm::two_pi<float>() * (float)segment / (float)segmentCount; // angle around the Y axis [0, 2pi]
				data.positions.push_back({
					radius * glm::cos(theta),
					y,
					radius * glm::sin(theta)
				});
			}
		}

		// Top circle
		for (int i = segmentCount - 1; i >= 0; i--)
			data.indices.push_back(i);
		data.polygonSizes.push_back(segmentCount);
		// Middle rectangles
		for (int segment = 0; segment < segmentCount; segment++) {
			// 1 0
			// 3 2
			int next = (segment + 1) % segmentCount;
			int point0 = segment;
			int point1 = next;
			int point2 = segmentCount + segment;
			int point3 = segmentCount + next;

			data.indices.push_back(point0);
			data.indices.push_back(point1);
			data.indices.push_back(point3);
			data.indices.push_back(point2);
			data.polygonSizes.push_back(4);
		}
		// Bottom circle
		for (int i = 0; i < segmentCount; i++)
			data.indices.push_back(segmentCount + i);
		data.polygonSizes.push_back(segmentCount);

		data.m_SmoothingMode = SmoothingMode::SmoothByAngle;
		data.CalculateNormals();
		return data;
	}


	MeshData MeshData::CreateCone(uint32_t segmentCount)
	{
		if (segmentCount < 3) segmentCount = 3;

		MeshData data;
		float radius = 0.5f;
		for (int segment = 0; segment < segmentCount; segment++)
		{
			float theta = glm::two_pi<float>() * (float)segment / (float)segmentCount; // angle around the Y axis [0, 2pi]
			data.positions.push_back({
				radius * glm::cos(theta),
				-0.5f,
				radius * glm::sin(theta)
				});
		}
		data.positions.push_back({ 0.0f, 0.5f, 0.0f }); // Top vertex

		// Trinagles
		int topIndex = (int)data.positions.size() - 1;
		for (int segment = 0; segment < segmentCount; segment++) {
			int next = (segment + 1) % segmentCount;
			int point0 = segment;
			int point1 = next;

			data.indices.push_back(point1);
			data.indices.push_back(point0);
			data.indices.push_back(topIndex);
			data.polygonSizes.push_back(3);
		}
		// Bottom circle
		for (int i = 0; i < segmentCount; i++)
			data.indices.push_back(i);
		data.polygonSizes.push_back(segmentCount);

		data.m_SmoothingMode = SmoothingMode::SmoothByAngle;
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