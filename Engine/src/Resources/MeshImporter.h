#pragma once

#include "Core/Core.h"
#include "Renderer/3D/Mesh.h"

namespace ft {

	class MeshImporter
	{
	public:
		static std::optional<Mesh> ImportMesh(const std::string& path, bool isStatic = false);
	private:
		MeshImporter() = default;

		struct Face
		{
			std::vector<uint32_t> vertexIndices;
			std::vector<uint32_t> uvIndices;
			std::vector<uint32_t> normalIndices;
		};
	};
}

