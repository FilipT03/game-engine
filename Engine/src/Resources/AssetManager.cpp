#include "pch.h"
#include "AssetManager.h"

namespace ft {
	std::unordered_map<std::string, std::weak_ptr<Texture>> AssetManager::s_Textures;

	std::shared_ptr<Texture> AssetManager::LoadTexture(const std::string& path)
	{
		auto it = s_Textures.find(path);
		if (it != s_Textures.end())
			if (auto existing = it->second.lock())
				return existing;
		
		
		std::shared_ptr<Texture> texture = std::shared_ptr<Texture>(Texture::Create(path));
		s_Textures[path] = texture;
		return texture;
	}

	std::shared_ptr<Texture> AssetManager::GetTexture(const std::string& path)
	{
		auto it = s_Textures.find(path);
		if (it != s_Textures.end())
			if (auto existing = it->second.lock())
				return existing;
		return nullptr;
	}
}