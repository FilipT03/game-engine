#include "MeshUtil.h"
#include <unordered_map>
#include <unordered_set>

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

void MeshUtil::MoveFaces(ft::MeshData& mesh, const std::set<uint32_t>& faces, const glm::vec3& offset)
{
	std::set<uint32_t> affectedVertices;
	uint32_t v = 0;
	for (size_t f = 0; f < mesh.polygonSizes.size(); v += mesh.polygonSizes[f], f++) {
		if (!faces.contains(f))
			continue;
		for (int i = 0; i < mesh.polygonSizes[f]; i++) {
			uint32_t vertexIndex = mesh.indices[v + i];
			affectedVertices.insert(vertexIndex);
		}
	}
	for (uint32_t vertexIndex : affectedVertices)
		mesh.positions[vertexIndex] += offset;
}

void MeshUtil::ScaleFaces(ft::MeshData& mesh, const std::set<uint32_t>& faces, const glm::vec3& scale, bool locked = false)
{
	std::set<uint32_t> affectedVertices;
	uint32_t v = 0;
	for (size_t f = 0; f < mesh.polygonSizes.size(); v += mesh.polygonSizes[f], f++) {
		if (!faces.contains(f))
			continue;
		for (int i = 0; i < mesh.polygonSizes[f]; i++) {
			uint32_t vertexIndex = mesh.indices[v + i];
			affectedVertices.insert(vertexIndex);
		}
	}
	glm::vec3 centroid(0.0f);
	for (uint32_t vertexIndex : affectedVertices)
		centroid += mesh.positions[vertexIndex];
	centroid /= affectedVertices.size();
	for (uint32_t vertexIndex : affectedVertices) {
		glm::vec3 dir = mesh.positions[vertexIndex] - centroid;
		mesh.positions[vertexIndex] = centroid + dir * scale;
	}
}

void MeshUtil::RotateFaces(ft::MeshData& mesh, const std::set<uint32_t>& faces, const glm::mat4& rotationMatrix)
{
	std::set<uint32_t> affectedVertices;
	uint32_t v = 0;
	for (size_t f = 0; f < mesh.polygonSizes.size(); v += mesh.polygonSizes[f], f++) {
		if (!faces.contains(f))
			continue;
		for (int i = 0; i < mesh.polygonSizes[f]; i++) {
			uint32_t vertexIndex = mesh.indices[v + i];
			affectedVertices.insert(vertexIndex);
		}
	}
	glm::vec3 centroid(0.0f);
	for (uint32_t vertexIndex : affectedVertices)
		centroid += mesh.positions[vertexIndex];
	centroid /= affectedVertices.size();
	for (uint32_t vertexIndex : affectedVertices) {
		glm::vec3 localPos = mesh.positions[vertexIndex] - centroid;
		glm::vec3 rotatedPos = glm::vec3(rotationMatrix * glm::vec4(localPos, 1.0f));
		mesh.positions[vertexIndex] = centroid + rotatedPos;
	}
	mesh.CalculateNormals();
}

void MeshUtil::ExtrudeFaces(ft::MeshData& mesh, const std::set<uint32_t>& faces, const glm::vec3& offset)
{	std::set<uint32_t> affectedVertices;
	uint32_t v = 0;
	for (size_t f = 0; f < mesh.polygonSizes.size(); v += mesh.polygonSizes[f], f++) {
		if (!faces.contains(f))
			continue;
		for (int i = 0; i < mesh.polygonSizes[f]; i++) {
			uint32_t vertexIndex = mesh.indices[v + i];
			affectedVertices.insert(vertexIndex);
		}
	}
	std::unordered_map<uint32_t, uint32_t> oldToNewVertex;
	// Adding all new vertices
	for (uint32_t vertexIndex : affectedVertices) {
		mesh.positions.push_back(mesh.positions[vertexIndex] + offset);
		oldToNewVertex[vertexIndex] = mesh.positions.size() - 1;
	}
	std::map<std::pair<uint32_t, uint32_t>, int> edgeCounts;

	v = 0;
	for (size_t f = 0; f < mesh.polygonSizes.size(); v += mesh.polygonSizes[f], f++) {
		if (!faces.contains(f))
			continue;
		uint32_t polygonSize = mesh.polygonSizes[f];
		for (int i = 0; i < polygonSize; i++) {
			uint32_t v1 = mesh.indices[v + i];
			uint32_t v2 = mesh.indices[v + (i + 1) % polygonSize];
			std::pair<uint32_t, uint32_t> edge = { std::min(v1, v2), std::max(v1, v2) };
			edgeCounts[edge]++;
		}
	}

	std::vector<uint32_t> newIndices;
	std::vector<uint32_t> newPolygonSizes;
	v = 0;
	for (size_t f = 0; f < mesh.polygonSizes.size(); v += mesh.polygonSizes[f], f++) {
		if (!faces.contains(f))
			continue;
		uint32_t polygonSize = mesh.polygonSizes[f];
		for (int i = 0; i < polygonSize; i++) {
			uint32_t v1 = mesh.indices[v + i];
			uint32_t v2 = mesh.indices[v + (i + 1) % polygonSize];
			std::pair<uint32_t, uint32_t> edge = { std::min(v1, v2), std::max(v1, v2) };
			// 1 -> boundary edge
			// 2 -> internal edge
			if (edgeCounts[edge] == 1) {
				newIndices.push_back(v1);
				newIndices.push_back(v2);
				newIndices.push_back(oldToNewVertex[v2]);
				newIndices.push_back(oldToNewVertex[v1]);
				newPolygonSizes.push_back(4);
			}
		}
		for (int i = 0; i < polygonSize; i++) {
			uint32_t vertexIndex = mesh.indices[v + i];
			mesh.indices[v + i] = oldToNewVertex[vertexIndex];
		}
	}

	// Indices for rectangles
	for (size_t i = 0; i < newIndices.size(); i++)
		mesh.indices.push_back(newIndices[i]);
	// Polygon sizes for rectangles
	for (size_t i = 0; i < newPolygonSizes.size(); i++)
		mesh.polygonSizes.push_back(newPolygonSizes[i]);

	mesh.CalculateNormals();
}

ft::MeshData MeshUtil::CreateGrid(int size, float spacing) {
	ft::MeshData gridData;
	for (int i = -size; i <= size; i++) {
		if (i == 0)
			continue;
		float x = i * spacing;
		gridData.positions.push_back(glm::vec3(i * spacing, 0.0f, -size * spacing));
		gridData.positions.push_back(glm::vec3(i * spacing, 0.0f, size * spacing));
		gridData.indices.push_back(gridData.indices.size());
		gridData.indices.push_back(gridData.indices.size());
		gridData.faceNormals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		gridData.polygonSizes.push_back(2);
	}
	for (int j = -size; j <= size; j++) {
		if (j == 0)
			continue;
		gridData.positions.push_back(glm::vec3(-size * spacing, 0.0f, j * spacing));
		gridData.positions.push_back(glm::vec3(size * spacing, 0.0f, j * spacing));
		gridData.indices.push_back(gridData.indices.size());
		gridData.indices.push_back(gridData.indices.size());
		gridData.faceNormals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		gridData.polygonSizes.push_back(2);
	}
	return gridData;
}

