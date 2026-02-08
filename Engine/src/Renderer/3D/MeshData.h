#pragma once

#include "Core/Core.h"

namespace ft {
	enum class SmoothingMode {
		Flat,
		Smooth,
		SmoothByAngle
	};

	class MeshData
	{
	public:
		std::vector<glm::vec3> positions;
		std::vector<uint32_t> indices;
		std::vector<uint32_t> polygonSizes; // Number of points in each polygon
		std::vector<glm::vec3> faceNormals;

		std::vector<glm::vec3> vertexNormals;
		std::vector<std::vector<uint32_t>> vertexToFaceMap; // Maps each vertex to the faces it belongs to


		void CalculateNormals();
		void CalculateVertexToFaceMap();
		void Clear();

		SmoothingMode GetSmoothingMode() const { return m_SmoothingMode; }
		void SetSmoothingMode(SmoothingMode mode) { 
			m_SmoothingMode = mode;
			CalculateNormals();
		}

		static MeshData CreateCube();
		static MeshData CreateSphere(uint32_t segmentCount, uint32_t ringCount);
		static MeshData CreateCylinder(uint32_t segmentCount, float height);
		static MeshData CreateTetrahedron();
		static MeshData CreatePlane();
	private:
		SmoothingMode m_SmoothingMode = SmoothingMode::Flat;
	};
}

