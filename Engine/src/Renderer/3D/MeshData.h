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

		std::vector<glm::vec3> cornerNormals;
		std::vector<std::vector<uint32_t>> vertexToFaceMap; // Maps each vertex to the faces it belongs to


		void CalculateNormals();
		void CalculateVertexToFaceMap();
		void Clear();

		SmoothingMode GetSmoothingMode() const { return m_SmoothingMode; }
		bool IsSmoothingEnabled() const { return m_SmoothingMode != SmoothingMode::Flat; }
		void SetSmoothingMode(SmoothingMode mode) { 
			m_SmoothingMode = mode;
			CalculateNormals();
		}
		float GetSmoothingThreshold() const { return m_SmoothingTheshold; }
		void SetSmoothingThreshold(float threshold) { 
			m_SmoothingTheshold = threshold;
			CalculateNormals();
		}

		static MeshData CreateCube();
		static MeshData CreateSphere(uint32_t segmentCount, uint32_t ringCount);
		static MeshData CreateCylinder(uint32_t segmentCount);
		static MeshData CreateCone(uint32_t segmentCount);
		static MeshData CreatePlane();
	private:
		SmoothingMode m_SmoothingMode = SmoothingMode::Flat;
		float m_SmoothingTheshold = 30.0f;
	};
}

