#include "MeshUtil.h"
#include <unordered_map>
#include <unordered_set>

void MeshUtil::MergeMeshes(const std::vector<ft::Mesh*>& sourceMeshes, ft::MeshData& targetMesh)
{
	targetMesh.Clear();
	for (const ft::Mesh* mesh : sourceMeshes) {
		if (!mesh) 
			continue;
		const ft::MeshData* meshData = mesh->GetData();
		if (!meshData) 
			continue;

		uint32_t vertexOffset = targetMesh.positions.size();
		const glm::mat4& modelMatrix = mesh->modelMatrix;
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));


		targetMesh.positions.reserve(targetMesh.positions.size() + meshData->positions.size());
		for (const glm::vec3& pos : meshData->positions)
			targetMesh.positions.push_back(glm::vec3(modelMatrix * glm::vec4(pos, 1.0f)));

		targetMesh.indices.reserve(targetMesh.indices.size() + meshData->indices.size());
		for (uint32_t index : meshData->indices)
			targetMesh.indices.push_back(index + vertexOffset);

		targetMesh.polygonSizes.insert(targetMesh.polygonSizes.end(), meshData->polygonSizes.begin(), meshData->polygonSizes.end());

		targetMesh.faceNormals.reserve(targetMesh.faceNormals.size() + meshData->faceNormals.size());
		for (const glm::vec3& normal : meshData->faceNormals)
			targetMesh.faceNormals.push_back(glm::normalize(normalMatrix * normal));

		targetMesh.cornerNormals.reserve(targetMesh.cornerNormals.size() + meshData->cornerNormals.size());
		for (const glm::vec3& normal : meshData->cornerNormals)
			targetMesh.cornerNormals.push_back(glm::normalize(normalMatrix * normal));
	}

	targetMesh.CalculateVertexToFaceMap();
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
			uint32_t sourceVertex = sourceMesh.indices[v + i];
			if (uniqueVertices.contains(sourceVertex))
				targetMesh.indices.push_back(uniqueVertices[sourceVertex]);
			else {
				targetMesh.positions.push_back(sourceMesh.positions[sourceVertex]);
				uint32_t newVertexIndex = targetMesh.positions.size() - 1; 
				targetMesh.indices.push_back(newVertexIndex); 
				uniqueVertices[sourceVertex] = newVertexIndex; 
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
	mesh.CalculateNormals();
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
	mesh.CalculateNormals();
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

void MeshUtil::CutFace(ft::MeshData& mesh, uint32_t faceIndex, uint32_t vertexA, uint32_t vertexB)
{
	if (vertexA == vertexB)
		return;

	uint32_t v = 0;
	for (size_t f = 0; f < faceIndex; f++)
		v += mesh.polygonSizes[f];
	uint32_t polygonSize = mesh.polygonSizes[faceIndex];
	
	uint32_t indexA = UINT32_MAX, indexB = UINT32_MAX;
	for (size_t i = 0; i < polygonSize; i++) {
		uint32_t currentVertex = mesh.indices[v + i];
		if (currentVertex == vertexA)
			indexA = i;
		else if (currentVertex == vertexB)
			indexB = i;
	}
	if (indexA == UINT32_MAX || indexB == UINT32_MAX) {
		FT_ERROR("One or both vertices not found in the specified face for cutting.");
		return;
	}
	bool isEdge =
		(indexA + 1) % polygonSize == indexB ||
		(indexB + 1) % polygonSize == indexA;
	if (isEdge) // Split edge
	{
		mesh.positions.push_back((mesh.positions[vertexA] + mesh.positions[vertexB]) * 0.5f);
		uint32_t newVertexIndex = mesh.positions.size() - 1;
		if ((indexA + 1) % polygonSize == indexB)
			mesh.indices.insert(mesh.indices.begin() + v + indexB, newVertexIndex);
		else
			mesh.indices.insert(mesh.indices.begin() + v + indexA, newVertexIndex);
		mesh.polygonSizes[faceIndex]++;
		
		std::pair<uint32_t, uint32_t> edge = { std::min(vertexA, vertexB), std::max(vertexA, vertexB) };
		v = 0;
		for (uint32_t f = 0; f < mesh.polygonSizes.size(); ++f) {
			if (f == faceIndex)	{
				v += mesh.polygonSizes[f];
				continue;
			}
			polygonSize = mesh.polygonSizes[f];
			for (int i = 0; i < polygonSize; ++i) {
				uint32_t v1 = mesh.indices[v + i];
				uint32_t v2 = mesh.indices[v + (i + 1) % polygonSize];
				if (edge.first == std::min(v1, v2) && edge.second == std::max(v1, v2)) {
					mesh.indices.insert(mesh.indices.begin() + v + i + 1, newVertexIndex);
					mesh.polygonSizes[f]++;
					polygonSize++;
					break;
				}
			}
			v += polygonSize;
		}
	}
	else // Connect vertices
	{
		if (indexA > indexB)
			std::swap(indexA, indexB);
		std::vector<uint32_t> indicesA, indicesB;
	
		for (uint32_t i = indexA; i <= indexB; i++) {
			uint32_t currentVertex = mesh.indices[v + i];
			indicesA.push_back(currentVertex);
		}
		for (uint32_t i = indexB; i < polygonSize; i++) {
			uint32_t currentVertex = mesh.indices[v + i];
			indicesB.push_back(currentVertex);
		}
		for (uint32_t i = 0; i <= indexA; i++) {
			uint32_t currentVertex = mesh.indices[v + i];
			indicesB.push_back(currentVertex);
		}

		mesh.indices.erase(mesh.indices.begin() + v, mesh.indices.begin() + v + polygonSize);
		mesh.polygonSizes.erase(mesh.polygonSizes.begin() + faceIndex);

		mesh.indices.insert(mesh.indices.begin() + v, indicesA.begin(), indicesA.end());
		mesh.indices.insert(mesh.indices.begin() + v + indicesA.size(), indicesB.begin(), indicesB.end());
		mesh.polygonSizes.insert(mesh.polygonSizes.begin() + faceIndex,
			{ static_cast<uint32_t>(indicesA.size()), static_cast<uint32_t>(indicesB.size())}
		);
	}

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

bool MeshUtil::FindClosestVertexToMouse(const ft::Mesh* mesh, uint32_t faceIndex, const glm::vec2& mousePosition, const ft::WorldCamera3D* camera, uint32_t& closestVertexIndex, float maxPixelDistance)
{
	const ft::MeshData* data = mesh->GetData();
	if (!data || faceIndex >= data->polygonSizes.size())
		return false;

	size_t v = 0;
	for (size_t f = 0; f < faceIndex; f++)
		v += data->polygonSizes[f];

	uint32_t polygonSize = data->polygonSizes[faceIndex];
	
	float closestDistance = maxPixelDistance;
	bool found = false;

	for (uint32_t i = 0; i < polygonSize; i++) 
	{
		uint32_t vertexIndex = data->indices[v + i];
		const glm::vec3& localPosition = data->positions[vertexIndex];
		const glm::vec3 worldPosition = glm::vec3(mesh->modelMatrix * glm::vec4(localPosition, 1.0f));

		glm::vec2 screenPosition = camera->WorldToScreen(worldPosition);
		float screenDistance = glm::length(screenPosition - mousePosition);

		if (screenDistance < closestDistance)
		{
			closestDistance = screenDistance;
			closestVertexIndex = vertexIndex;
			found = true;
		}
	}

	return found;
}

