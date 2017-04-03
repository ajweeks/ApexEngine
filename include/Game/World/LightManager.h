#pragma once

#include "World.h"

#include "Apex\IO\MouseListener.h"

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\RenderTexture.hpp>
#include <SFML\Graphics\Shader.hpp>
#include <SFML\Graphics\Sprite.hpp>

#include <vector>

class LightManager : public apex::MouseListener
{
public:
	LightManager(World& world, const std::string& directory);
	virtual ~LightManager();

	LightManager(const LightManager&) = delete;
	LightManager& operator=(const LightManager&) = delete;

	static void LoadShaders();

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

	void OnWindowResize(sf::Vector2u windowSize);

	void LoadLightData();
	void SaveLightData();

	void SetShowingEditor(bool showingEditor);
	bool IsShowingEditor() const;

	virtual bool OnButtonPress(sf::Event::MouseButtonEvent buttonEvent);
	virtual void OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent);
	virtual void OnScroll(sf::Event::MouseWheelScrollEvent scrollEvent) {};

private:
	static const std::string LIGHTMAP_FILENAME;

	void DrawEditor(sf::RenderTarget& target, sf::RenderStates states);

	static sf::Shader s_LightingShader;
	static bool s_LoadedLightingShader;
	
	struct ApexLight
	{
		float blur;
		sf::Color color;
		bool flickers; // Not currently supported! (TODO: Add dynamic lights)
		float opacity;
		sf::Vector2f position;
		float radius;
	};
	std::vector<ApexLight> m_StaticLights;
	sf::RenderTexture m_LightmapTexture;
	sf::Sprite m_LightmapSprite;
	sf::Color m_AmbientColor;

	std::string m_Directory;

	int m_BuildingIndex;
	World& m_World;

	bool m_NeedUpdate = true;

	// Editor variables:
	bool m_IsShowingEditor;
	int m_SelectedLightIndex = -1;
	int m_CurrentLightDraggingIndex = -1;
	int m_CurrentLightRadiusIndex = -1;
	int m_CurrentLightBlurIndex = -1;
	sf::Vector2f m_StartPosDrag;
	sf::Vector2f m_MousePosDragStartWorld;
	float m_RadiusDragStart;
	float m_BlurDragStart;
};
