#pragma once

#include "Core/Core.h"

namespace ft {

	class MeshData
	{
	public:
		std::vector<glm::vec3> positions;
		std::vector<uint32_t> indices;
		std::vector<uint32_t> polygonSizes; // Number of points in each polygon
		std::vector<glm::vec3> faceNormals;

		void CalculateNormals(bool smooth = false);
		void Clear();

		static MeshData CreateCube();
		static MeshData CreateSphere(uint32_t segmentCount, uint32_t stackCount);
		static MeshData CreateCylinder(uint32_t segmentCount, float height);
		static MeshData CreateTetrahedron();
		static MeshData CreatePlane();
	};
}

