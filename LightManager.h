#pragma once

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\RenderTexture.hpp>
#include <SFML\Graphics\Shader.hpp>
#include <SFML\Graphics\Sprite.hpp>

#include <vector>

class LightManager
{
public:
	LightManager(int levelIndex);
	virtual ~LightManager();

	LightManager(const LightManager&) = delete;
	LightManager& operator=(const LightManager&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

	void OnWindowResize(sf::Vector2u windowSize);

	void LoadShader();
	void LoadLights();

	void SetLevelIndex(int levelIndex);

private:
	static sf::Color StringToColor(std::string string);
	static sf::Vector2f StringToVector2f(std::string string);;


	struct ApexLight
	{
		float blur;
		sf::Color color;
		bool flickers;
		float opacity;
		sf::Vector2f position;
		float radius;
	};
	std::vector<ApexLight> m_Lights;
	sf::RenderTexture m_LightmapTexture;
	sf::Sprite m_LightmapSprite;

	sf::Color m_AmbientColor;

	sf::Shader m_LightingShader;

	int m_LevelIndex;
};
