#pragma once

#include "Core/Core.h"
#include "Renderer/Texture.h"

namespace ft {

	class AssetManager
	{
	public:
		static std::shared_ptr<Texture> LoadTexture(const std::string& path);
		static std::shared_ptr<Texture> GetTexture(const std::string& path);
	private:
		static std::unordered_map<std::string, std::weak_ptr<Texture>> s_Textures;
	};
}

