#include "pch.h"
#include "MeshImporter.h"

#include<fstream>
#include<optional>

namespace ft {

	std::optional<Mesh> MeshImporter::ImportMesh(const std::string& path, bool isStatic)
	{
		MeshData data;
		std::string extension = path.substr(path.find_last_of(".") + 1);
		if (extension != "obj") {
			FT_ENGINE_ERROR("Unsupported mesh format: {0}", extension);
			return std::nullopt;
		}
		std::ifstream file(path);
		if (file.fail()) {
			FT_ENGINE_ERROR("Failed to open mesh file: {0}", path);
			return std::nullopt;
		}
		std::string line;
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<Face> faces;
		while (!file.eof()) {
			std::getline(file, line);
			std::istringstream ss(line);
			std::string prefix;
			ss >> prefix;
			if (prefix == "v") {
				glm::vec3 vertex;
				ss >> vertex.x >> vertex.y >> vertex.z;
				vertices.push_back(vertex);
			}
			else if (prefix == "f") {
				std::string vertexStr;
				Face face;
				while (ss >> vertexStr) {
					size_t pos = vertexStr.find('/');
					uint32_t vertexIndex = std::stoi(vertexStr.substr(0, pos)) - 1;
					face.vertexIndices.push_back(vertexIndex);
					pos = vertexStr.find('/', pos + 1);
					uint32_t normalIndex = std::stoi(vertexStr.substr(pos + 1)) - 1;
					face.normalIndices.push_back(normalIndex);
				}
				faces.push_back(face);
			}
			else if (prefix == "vn") {
				glm::vec3 normal;
				ss >> normal.x >> normal.y >> normal.z;
				normals.push_back(normal);
			}
		}

		// Construct MeshData from the parsed data
		for (const glm::vec3& vertex : vertices)
			data.positions.push_back(vertex);
		for (const Face& face : faces) {
			uint32_t polygonSize = face.vertexIndices.size();
			data.polygonSizes.push_back(polygonSize);
			for (int i = 0; i < polygonSize; i++) {
				uint32_t normalIndex = face.normalIndices[i];
				data.indices.push_back(face.vertexIndices[i]);
				data.cornerNormals.push_back(normals[normalIndex]);
			}
		}
		data.CalculateFaceNormals();
		data.SetUseCornerNormals(true); // Use corner normals regardless of smoothing mode
		return Mesh(data, Transform3D(), glm::vec4(1.0f), isStatic);
	}
}