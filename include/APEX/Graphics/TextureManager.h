#pragma once

#include <SFML\Graphics\Texture.hpp>

#include <memory>

namespace apex
{
	class TextureManager
	{
	public:
		static void AddTexture(size_t index, const std::string& filePath);
		static sf::Texture* GetTexture(size_t index);

	private:
		TextureManager();
		virtual ~TextureManager();
		TextureManager(const TextureManager&) = delete;
		TextureManager& operator=(const TextureManager&) = delete;

		static std::vector<std::unique_ptr<sf::Texture>> s_Textures;

	};
} // namespace apex
