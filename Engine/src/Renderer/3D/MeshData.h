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
		void CalculateFaceNormals();
		void CalculateVertexToFaceMap();
		void Clear();

		SmoothingMode GetSmoothingMode() const { return m_SmoothingMode; }
		bool IsSmoothingEnabled() const { return m_SmoothingMode != SmoothingMode::Flat; }
		void SetSmoothingMode(SmoothingMode mode) { 
			m_SmoothingMode = mode;
			m_UseCornerNormals = false;
			CalculateNormals();
		}
		float GetSmoothingThreshold() const { return m_SmoothingTheshold; }
		void SetSmoothingThreshold(float threshold) { 
			m_SmoothingTheshold = threshold;
			CalculateNormals();
		}
		void SetUseCornerNormals(bool value) { m_UseCornerNormals = value; }
		bool UsesCornerNormals() const { return m_UseCornerNormals; }

		static MeshData CreateCube();
		static MeshData CreateSphere(uint32_t segmentCount, uint32_t ringCount);
		static MeshData CreateCylinder(uint32_t segmentCount);
		static MeshData CreateCone(uint32_t segmentCount);
		static MeshData CreatePlane();
		static MeshData CreateLine(const glm::vec3& start, const glm::vec3& end);
	private:
		SmoothingMode m_SmoothingMode = SmoothingMode::Flat;
		bool m_UseCornerNormals = false;
		float m_SmoothingTheshold = 30.0f;
	};
}

