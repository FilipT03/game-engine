#include "GameEngine.h"

#include <set>

#pragma once
class MeshUtil
{
public:
	static void CopyMesh(const ft::MeshData& sourceMesh, ft::MeshData& targetMesh);
	static void MergeMeshes(const std::vector<ft::MeshData>& sourceMeshes, ft::MeshData& targetMesh);
	static void CreateFromFaces(const ft::MeshData& sourceMesh, ft::MeshData& targetMesh, const std::set<uint32_t>& faces);
};

