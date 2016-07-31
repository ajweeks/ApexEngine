#pragma once

#include <SFML\Graphics\Texture.hpp>

class TextureManager
{
public:
	enum Texture
	{
		DUST_PARTICLE,
		PLAYER,
		SHADOW,
		CURSOR_NORMAL, CURSOR_POINTER,
		AMMO, COIN,
		TITLE_COLOR, TITLE_SHADOW,
		SPEECH_BUBBLE,
		NPC,

		_LAST_ELEMENT
	};

	static void Initialize();
	static void Destroy();

	static sf::Texture* GetTexture(Texture texture);

private:
	TextureManager();
	virtual ~TextureManager();
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;
	
	static void LoadTexture(Texture texture, const std::string& filePath);

	static std::vector<sf::Texture*> s_Textures;
	static bool s_initialized;

};
