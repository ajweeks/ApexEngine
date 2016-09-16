#pragma once

#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\Text.hpp>
#include <SFML\System\Time.hpp>

class World;

class HUD
{
public:
	HUD(World& world);
	virtual ~HUD();

	HUD(const HUD&) = delete;
	HUD& operator=(const HUD&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

private:
	World& m_World;

	sf::Text m_BulletsInClip;
	sf::Text m_BulletsRemaining;

};
