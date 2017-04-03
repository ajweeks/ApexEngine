
#include "Game\HUD.h"
#include "Game\Game.h"
#include "Game\World\World.h"
#include "Game\World\Player.h"
#include "Game\World\Gun.h"

#include "Apex\ApexMain.h"

HUD::HUD(World& world) :
	m_World(world)
{
	const sf::Font& font = Game::FontOpenSans;
	m_BulletsInClip.setFont(font);
	m_BulletsRemaining.setFont(font);

	const unsigned int characterSize = 38;
	m_BulletsInClip.setCharacterSize(characterSize);
	m_BulletsRemaining.setCharacterSize(characterSize);

	const sf::Color fontColor = sf::Color(210, 210, 215);
	m_BulletsInClip.setFillColor(fontColor);
	m_BulletsRemaining.setFillColor(fontColor);
}

HUD::~HUD()
{
}

void HUD::Tick(sf::Time elapsed)
{
	//Gun& gun = m_World.GetPlayer()->GetGun();
	//m_BulletsInClip.setString(std::to_string(gun.GetBulletsInClip()));
	//m_BulletsRemaining.setString(" / " + std::to_string(gun.GetBulletsRemaining()));

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
