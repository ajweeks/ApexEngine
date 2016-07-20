#pragma once

#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\Text.hpp>
#include <SFML\System\Time.hpp>

class Level;

class HUD
{
public:
	HUD(Level* level);
	virtual ~HUD();

	HUD(const HUD&) = delete;
	HUD& operator=(const HUD&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

private:
	Level* m_Level = nullptr;

	sf::Text m_BulletsRemaining;
	sf::Text m_ClipsRemaining;
	sf::Text m_ClipSize;

};
