#include "GameEngine.h"

#include <set>

#pragma once
class MeshUtil
{
public:
	static void CopyMesh(const ft::MeshData& sourceMesh, ft::MeshData& targetMesh);
	static void MergeMeshes(const std::vector<ft::MeshData>& sourceMeshes, ft::MeshData& targetMesh);
	static void CreateFromFaces(const ft::MeshData& sourceMesh, ft::MeshData& targetMesh, const std::set<uint32_t>& faces);

	static void MoveFaces(ft::MeshData& mesh, const std::set<uint32_t>& faces, const glm::vec3& offset);
	static void ScaleFaces(ft::MeshData& mesh, const std::set<uint32_t>& faces, const glm::vec3& scale, bool locked);
	static void ExtrudeFaces(ft::MeshData& mesh, const std::set<uint32_t>& faces, const glm::vec3& offset);
	static void RotateFaces(ft::MeshData& mesh, const std::set<uint32_t>& faces, const glm::mat4& rotationMatrix);

	static ft::MeshData CreateGrid(int size, float spacing);
};

