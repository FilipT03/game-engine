#include "MeshUtil.h"

void MeshUtil::CopyMesh(const ft::MeshData& sourceMesh, ft::MeshData& targetMesh)
{
	targetMesh.Clear();
	targetMesh.SetSmoothingMode(sourceMesh.GetSmoothingMode()); 
	targetMesh.SetSmoothingThreshold(sourceMesh.GetSmoothingThreshold());
	targetMesh.positions = sourceMesh.positions;
	targetMesh.indices = sourceMesh.indices;
	targetMesh.polygonSizes = sourceMesh.polygonSizes;
	targetMesh.faceNormals = sourceMesh.faceNormals;
	targetMesh.cornerNormals = sourceMesh.cornerNormals;
}

void MeshUtil::MergeMeshes(const std::vector<ft::MeshData>& sourceMeshes, ft::MeshData& targetMesh)
{
	targetMesh.Clear();
	for (const ft::MeshData& mesh : sourceMeshes) {
		uint32_t vertexOffset = targetMesh.positions.size();
		targetMesh.positions.insert(targetMesh.positions.end(), mesh.positions.begin(), mesh.positions.end());
		targetMesh.indices.insert(targetMesh.indices.end(), mesh.indices.begin(), mesh.indices.end());
		targetMesh.polygonSizes.insert(targetMesh.polygonSizes.end(), mesh.polygonSizes.begin(), mesh.polygonSizes.end());
		targetMesh.faceNormals.insert(targetMesh.faceNormals.end(), mesh.faceNormals.begin(), mesh.faceNormals.end());
		targetMesh.cornerNormals.insert(targetMesh.cornerNormals.end(), mesh.cornerNormals.begin(), mesh.cornerNormals.end());
		for (size_t i = targetMesh.indices.size() - mesh.indices.size(); i < targetMesh.indices.size(); i++)
			targetMesh.indices[i] += vertexOffset;
	}
}

void MeshUtil::CreateFromFaces(const ft::MeshData& sourceMesh, ft::MeshData& targetMesh, const std::set<uint32_t>& faces)
{
	targetMesh.Clear();
	uint32_t v = 0;
	std::unordered_map<uint32_t, uint32_t> uniqueVertices;
	for (size_t f = 0; f < sourceMesh.polygonSizes.size(); v += sourceMesh.polygonSizes[f], f++) {
		if (!faces.contains(f))
			continue;
		uint32_t polygonSize = sourceMesh.polygonSizes[f];
		
		for (int i = 0; i < polygonSize; i++) { 
			uint32_t sourceIndex = sourceMesh.indices[v + i];
			if (uniqueVertices.contains(sourceIndex))
				targetMesh.indices.push_back(uniqueVertices[sourceIndex]);
			else {
				targetMesh.positions.push_back(sourceMesh.positions[sourceIndex]);
				uint32_t newVertexIndex = targetMesh.positions.size() - 1; 
				targetMesh.indices.push_back(newVertexIndex); 
				uniqueVertices[sourceIndex] = newVertexIndex; 
			}
		}
		targetMesh.polygonSizes.push_back(polygonSize);
		targetMesh.faceNormals.push_back(sourceMesh.faceNormals[f]);
	}
}