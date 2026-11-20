#include <GameEngine.h>
#include <set>

#include "ConstraintFrame.h"

#pragma once
class MeshUtil
{
public:
	static void MergeMeshes(const std::vector<ft::Mesh*>& sourceMeshes, ft::MeshData& targetMesh, const glm::mat4& targetModelMatrix);
	static void CreateFromFaces(const ft::MeshData& sourceMesh, ft::MeshData& targetMesh, const std::set<uint32_t>& faces);

	static void MoveFaces(ft::MeshData& mesh, const std::set<uint32_t>& faces, const glm::vec3& offset);
	static void ScaleFaces(ft::MeshData& mesh, const std::set<uint32_t>& faces, const float scale, const ConstraintFrame constraint, const glm::mat3& objectToWorld);
	static void ExtrudeFaces(ft::MeshData& mesh, const std::set<uint32_t>& faces, const glm::vec3& offset);
	static void RotateFaces(ft::MeshData& mesh, const std::set<uint32_t>& faces, const glm::mat4& rotationMatrix);
	static void CutFace(ft::MeshData& mesh, uint32_t faceIndex, uint32_t vertexA, uint32_t vertexB);

	static glm::vec3 GetAverageNormal(ft::MeshData& mesh, const std::set<uint32_t>& faces);
	static glm::vec3 GetTangent(ft::MeshData& mesh, const std::set<uint32_t>& faces, glm::vec3 normal, glm::vec3 preferredAxis);
	static glm::vec3 GetCentroid(ft::MeshData& mesh, const std::set<uint32_t>& faces);
	

	static ft::MeshData CreateGrid(int size, float spacing);

	static bool FindClosestVertexToMouse(const ft::Mesh* mesh, uint32_t faceIndex, const glm::vec2& mousePosition, const ft::WorldCamera3D* camera, uint32_t& closestVertexIndex, float maxPixelDistance);
};

