
#include "Gun.h"
#include "Bullet.h"
#include "PhysicsActor.h"
#include "Level.h"
#include "ApexMouse.h"
#include "ApexAudio.h"
#include "Player.h"
#include "Game.h"

Gun::Gun(Level* level, sf::Vector2f position, Player* playerHolding) :
	Entity(level, position, Type::GUN, this),
	m_Level(level),
	m_PlayerHolding(playerHolding)
{
	m_BulletManager = level->GetBulletManager();
	m_Direction = 0.0f;

	m_RectShape.setFillColor(sf::Color::Blue);
	m_RectShape.setSize(sf::Vector2f(22, 10));
	m_RectShape.setPosition(sf::Vector2f(8, -5));

	Reset();
}

Gun::~Gun()
{
}

void Gun::Reset()
{
	m_ClipSize = 25;
	m_ClipsRemaining = 3;
	m_BulletsRemaining = m_ClipSize;
}

void Gun::Tick(sf::Time elapsed)
{
	if (m_PlayerHolding != nullptr)
	{
		const sf::Vector2f mousePos = static_cast<sf::Vector2f>(m_Level->GetGame()->GetMouseCoordsScreenSpace(m_Level->GetCurrentView()));
		const sf::Vector2f dPos = mousePos - m_Actor->GetPosition();
		m_Direction = atan2(dPos.y, dPos.x);
		
		m_Actor->SetPosition(m_PlayerHolding->GetPosition());

		const bool mouseInBounds = mousePos.x > 0 && mousePos.x < 2080 &&
			mousePos.y > 0 && mousePos.y < 1216;
		if (mouseInBounds && ApexMouse::IsButtonPressed(sf::Mouse::Button::Left))
		{
			Shoot();
		}
	}
}

void Gun::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	states.transform.translate(m_Actor->GetPosition()).rotate(m_Direction * (180.0f/ 3.1415f));
	target.draw(m_RectShape, states);
}

void Gun::Shoot()
{
	if (m_BulletsRemaining == 0)
	{
		if (m_ClipsRemaining == 0)
		{
			ApexAudio::PlaySoundEffect(ApexAudio::Sound::GUN_FIRE_EMPTY);
			return;
		}
		else
		{
			ApexAudio::PlaySoundEffect(ApexAudio::Sound::GUN_RELOAD);
			m_BulletsRemaining = m_ClipSize;
			--m_ClipsRemaining;
		}
	}

	sf::Vector2f posOffset = sf::Vector2f(cos(m_Direction) * 22, sin(m_Direction) * 22);
	Bullet* newBullet = new Bullet(m_Level, m_Actor->GetPosition() + posOffset, m_Direction, m_PlayerHolding->GetVelocity() / 2.0f);
	--m_BulletsRemaining;
	ApexAudio::PlaySoundEffect(ApexAudio::Sound::GUN_FIRE);
}
