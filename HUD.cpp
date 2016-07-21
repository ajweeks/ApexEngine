
#include "HUD.h"
#include "Level.h"
#include "Player.h"
#include "Gun.h"
#include "ApexMain.h"

HUD::HUD(Level* level) :
	m_Level(level)
{
	Gun& gun = m_Level->GetPlayer()->GetGun();

	const sf::Font& font = APEX->FontOpenSans;
	m_BulletsInClip.setFont(font);
	m_BulletsRemaining.setFont(font);

	const unsigned int characterSize = 38;
	m_BulletsInClip.setCharacterSize(characterSize);
	m_BulletsRemaining.setCharacterSize(characterSize);

	const sf::Color fontColor = sf::Color(210, 210, 215);
	m_BulletsInClip.setColor(fontColor);
	m_BulletsRemaining.setColor(fontColor);
}

HUD::~HUD()
{
}

void HUD::Tick(sf::Time elapsed)
{
	Gun& gun = m_Level->GetPlayer()->GetGun();
	m_BulletsInClip.setString(std::to_string(gun.GetBulletsInClip()));
	m_BulletsRemaining.setString(" / " + std::to_string(gun.GetBulletsRemaining()));

	const sf::Vector2u windowSize = APEX->GetWindowSize();
	const float padding = 18.0f;
	const sf::FloatRect brBounds = m_BulletsRemaining.getLocalBounds();
	m_BulletsRemaining.setPosition(sf::Vector2f(windowSize.x - (brBounds.width + padding), windowSize.y - (brBounds.height + padding)));
	const sf::FloatRect bicBounds = m_BulletsInClip.getLocalBounds();
	m_BulletsInClip.setPosition(sf::Vector2f(windowSize.x - (brBounds.width + bicBounds.width + padding), windowSize.y - (bicBounds.height + padding)));
}

void HUD::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	target.draw(m_BulletsInClip, states);
	target.draw(m_BulletsRemaining, states);
}
