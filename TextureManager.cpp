
#include "TextureManager.h"
#include "ApexMain.h"

bool TextureManager::s_initialized = false;
std::vector<sf::Texture*> TextureManager::s_Textures;

TextureManager::TextureManager()
{}

TextureManager::~TextureManager()
{}

void TextureManager::Initialize()
{
	if (s_initialized) return;

	s_Textures.resize(Texture::_LAST_ELEMENT, nullptr);

	LoadTexture(Texture::DUST_PARTICLE, "resources/dust-explosion.png");
	LoadTexture(Texture::SMALL_MARIO, "resources/small-mario.png");
	LoadTexture(Texture::SHADOW, "resources/shadow.png");

	LoadTexture(Texture::CURSOR_NORMAL, "resources/cursor-normal-x2.png");
	LoadTexture(Texture::CURSOR_POINTER, "resources/cursor-point-x2.png");

	LoadTexture(Texture::AMMO, "resources/ammo.png");

	LoadTexture(Texture::TITLE_COLOR, "resources/title-color.png");
	LoadTexture(Texture::TITLE_SHADOW, "resources/title-shadow.png");

	s_initialized = true;
}

void TextureManager::Destroy()
{
	for (size_t i = 0; i < s_Textures.size(); ++i)
	{
		delete s_Textures[i];
	}
	s_Textures.clear();

	s_initialized = false;
}

void TextureManager::LoadTexture(Texture texture, const std::string& filePath)
{
	s_Textures[texture] = new sf::Texture();
	if (!s_Textures[texture]->loadFromFile(filePath))
	{
		ApexOutputDebugString("Could not load texture: " + filePath + "\n");
	}
}

sf::Texture* TextureManager::GetTexture(Texture texture)
{
	if (!s_initialized) return nullptr;
	return s_Textures[texture];
}
