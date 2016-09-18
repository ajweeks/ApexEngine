
#include <APEX\Graphics\TextureManager.h>
#include <APEX\ApexMain.h>

namespace apex
{
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

		LoadTexture(Texture::LIQWIDICE_GAMES_LOGO_SCREEN, "resources/images/title-card.png");

		LoadTexture(Texture::GENERAL_TILES, "resources/images/tiles.png");

		LoadTexture(Texture::PLAYER, "resources/images/player.png");
		LoadTexture(Texture::DUST_PARTICLE, "resources/images/dust-explosion.png");
		LoadTexture(Texture::SHADOW, "resources/images/shadow.png");

		LoadTexture(Texture::CURSOR_NORMAL, "resources/images/cursor-normal-x2.png");
		LoadTexture(Texture::CURSOR_POINTER, "resources/images/cursor-point-x2.png");

		LoadTexture(Texture::AMMO, "resources/images/ammo.png");
		LoadTexture(Texture::COIN, "resources/images/coin.png");

		LoadTexture(Texture::TITLE_COLOR, "resources/images/title-color.png");
		LoadTexture(Texture::TITLE_SHADOW, "resources/images/title-shadow.png");

		LoadTexture(Texture::SPEECH_BUBBLE, "resources/images/speech-bubble.png");

		LoadTexture(Texture::NPC, "resources/images/npc.png");

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
			apex::PrintString("Could not load texture: " + filePath + "\n");
		}
	}

	sf::Texture* TextureManager::GetTexture(Texture texture)
	{
		if (!s_initialized) return nullptr;
		return s_Textures[texture];
	}
} // namespace apex
