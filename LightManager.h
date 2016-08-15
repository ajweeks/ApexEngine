#pragma once

#include "ApexMouseListener.h"

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\RenderTexture.hpp>
#include <SFML\Graphics\Shader.hpp>
#include <SFML\Graphics\Sprite.hpp>

#include <vector>

class World;

class LightManager : public ApexMouseListener
{
public:
	LightManager(int worldIndex, World* world);
	virtual ~LightManager();

	LightManager(const LightManager&) = delete;
	LightManager& operator=(const LightManager&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

	void OnWindowResize(sf::Vector2u windowSize);

	void LoadShader();
	void LoadLightData();
	void SaveLightData();

	void SetWorldIndex(int worldIndex);

	void SetShowingEditor(bool showingEditor);
	void ToggleShowingEditor();
	bool IsShowingEditor() const;

	virtual bool OnButtonPress(sf::Event::MouseButtonEvent buttonEvent);
	virtual void OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent);
	virtual void OnScroll(sf::Event::MouseWheelScrollEvent scrollEvent) {};

private:
	static sf::Color StringToColor(std::string string);
	static std::string ColorToString(sf::Color color);
	static sf::Vector2f StringToVector2f(std::string string);
	static std::string Vector2fToString(sf::Vector2f vector);

	void DrawEditor(sf::RenderTarget& target, sf::RenderStates states);

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

	int m_WorldIndex;
	World* m_World = nullptr;

	// Editor variables:
	bool m_IsShowingEditor;
	int m_CurrentLightDraggingIndex = -1;
	int m_CurrentLightRadiusIndex = -1;
	int m_CurrentLightBlurIndex = -1;
	int m_LastSelectedLightIndex = -1;
	sf::Vector2f m_MouseDragStart;
	sf::Vector2f m_PosDragStart;
	float m_RadiusDragStart;
	float m_BlurDragStart;
};
