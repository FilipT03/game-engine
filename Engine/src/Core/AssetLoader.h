#pragma once

#include "Core/Core.h"

namespace ft {

	class AssetLoader
	{
	public:
		static const std::string LoadTextFile(const std::string& relativePath);
	private:
		static const std::string s_AssetDirectory;
	};
}

