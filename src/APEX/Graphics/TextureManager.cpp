
#include <APEX\Graphics\TextureManager.h>
#include <APEX\ApexMain.h>

namespace apex
{
	std::vector<std::unique_ptr<sf::Texture>> TextureManager::s_Textures;

	TextureManager::TextureManager()
	{}

	TextureManager::~TextureManager()
	{}

	void TextureManager::AddTexture(size_t index, const std::string & filePath)
	{
		if (index >= s_Textures.size()) s_Textures.resize(index + 1);

		s_Textures[index] = std::unique_ptr<sf::Texture>(new sf::Texture());
		if (!s_Textures[index]->loadFromFile(filePath))
		{
			apex::PrintString("Could not load texture: " + filePath + "\n", apex::LogType::LOG_ERROR);
		}
	}

	sf::Texture* TextureManager::GetTexture(size_t index)
	{
		return s_Textures[index].get();
	}
} // namespace apex
