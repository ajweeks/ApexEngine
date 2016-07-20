
#include "HUD.h"
#include "Level.h"
#include "Player.h"
#include "Gun.h"
#include "ApexMain.h"

HUD::HUD(Level* level) :
	m_Level(level)
{
	Gun& gun = m_Level->GetPlayer()->GetGun();
	m_ClipSize.setString(std::to_string(gun.GetClipSize()));

	const sf::Vector2u windowSize = APEX->GetWindowSize();
	m_BulletsRemaining.setPosition(sf::Vector2f(windowSize - sf::Vector2u(120, 100)));
	m_ClipSize.setPosition(sf::Vector2f(windowSize - sf::Vector2u(80, 100)));
	m_ClipsRemaining.setPosition(sf::Vector2f(windowSize - sf::Vector2u(120, 50)));

	const sf::Font& font = APEX->FontOpenSans;
	m_BulletsRemaining.setFont(font);
	m_ClipsRemaining.setFont(font);
	m_ClipSize.setFont(font);

	const unsigned int characterSize = 35;
	m_BulletsRemaining.setCharacterSize(characterSize);
	m_ClipsRemaining.setCharacterSize(characterSize);
	m_ClipSize.setCharacterSize(characterSize);

	const sf::Color fontColor = sf::Color(210, 210, 215);
	m_BulletsRemaining.setColor(fontColor);
	m_ClipsRemaining.setColor(fontColor);
	m_ClipSize.setColor(fontColor);

}

HUD::~HUD()
{
}

void HUD::Tick(sf::Time elapsed)
{
	Gun& gun = m_Level->GetPlayer()->GetGun();
	m_BulletsRemaining.setString(std::to_string(gun.GetBulletsRemaining()));
	m_ClipsRemaining.setString(std::to_string(gun.GetClipsRemaining()));
	m_ClipSize.setString("/" + std::to_string(gun.GetClipSize()));
}

void HUD::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	target.draw(m_BulletsRemaining, states);
	target.draw(m_ClipsRemaining, states);
	target.draw(m_ClipSize, states);
}
