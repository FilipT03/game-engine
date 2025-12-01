#include "pch.h"
#include "AssetLoader.h"

#include "Core/Log.h"
#include <fstream>
#include <filesystem>

namespace ft {
	const std::string AssetLoader::s_AssetDirectory = "assets/";

	const std::string AssetLoader::LoadTextFile(const std::string& relativePath) {
		std::string fullPath = s_AssetDirectory + relativePath;
		std::ifstream fileStream(fullPath);
		std::string path = std::filesystem::absolute(fullPath).string();
		FT_ENGINE_CRITICAL("Loading file at path: {}", path);
		if (!fileStream.is_open()) {
			FT_ENGINE_ERROR("Failed to open file: {}", fullPath);
			static std::string emptyString;
			return "";
		}

		std::stringstream buffer;
		buffer << fileStream.rdbuf();
		fileStream.close();

		return buffer.str();
	}
}