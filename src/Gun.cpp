
#include "Gun.h"
#include "ApexMain.h"
#include "Bullet.h"
#include "PhysicsActor.h"
#include "World.h"
#include "ApexMouse.h"
#include "ApexAudio.h"
#include "Player.h"
#include "AmmoDrop.h"

const float Gun::RELOAD_SECONDS = 0.8f;

Gun::Gun(World& world, Map& map, sf::Vector2f position) :
	Entity(world, map, position, ActorID::GUN, this),
	m_BulletManager(world.GetBulletManager())
{
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
	m_BulletsInClip = m_ClipSize;
	m_BulletsRemaining = m_ClipSize * 2;
	m_ReloadingTimeRemaining = sf::Time::Zero;
}

bool Gun::Empty() const
{
	return m_BulletsRemaining == 0 && m_BulletsInClip == 0;
}

void Gun::AddAmmo(AmmoDrop* ammoDrop)
{
	m_BulletsRemaining += ammoDrop->GetBulletCount();
}

int Gun::GetBulletsInClip() const
{
	return m_BulletsInClip;
}

int Gun::GetBulletsRemaining() const
{
	return m_BulletsRemaining;
}

int Gun::GetClipSize() const
{
	return m_ClipSize;
}

bool Gun::OnButtonPress(sf::Event::MouseButtonEvent buttonEvent)
{
	if (m_World.IsPaused()) return true;
	if (APEX->DEBUGIsGamePaused()) return true;

	switch (buttonEvent.button)
	{
	case sf::Mouse::Button::Left:
	{
		Shoot();
		return false;
	} break;
	case sf::Mouse::Button::XButton1:
	{
		Reload();
	} break;
	}

	return true;
}

void Gun::OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent)
{
	if (m_World.IsPaused()) return;
	if (APEX->DEBUGIsGamePaused()) return;
}

void Gun::OnScroll(sf::Event::MouseWheelScrollEvent scrollEvent)
{
	if (m_World.IsPaused()) return;
	if (APEX->DEBUGIsGamePaused()) return;
}

float Gun::GetDirection() const
{
	return m_Direction;
}

void Gun::Tick(sf::Time elapsed)
{
	if (m_ReloadingTimeRemaining != sf::Time::Zero)
	{
		if (m_ReloadingTimeRemaining != sf::Time::Zero)
		{
			m_ReloadingTimeRemaining -= elapsed;
			if (m_ReloadingTimeRemaining < sf::Time::Zero)
				m_ReloadingTimeRemaining = sf::Time::Zero;
		}

		const sf::Vector2f mousePos = static_cast<sf::Vector2f>(APEX->GetMouseCoordsScreenSpace(m_World->GetCurrentView()));
		if (mousePos.x != -1.0f && mousePos.y != -1.0f) // only update our rotation when the mouse is inside the window
		{
			const sf::Vector2f dPos = mousePos - m_Actor->GetPosition();
			m_Direction = atan2(dPos.y, dPos.x);
		}
		
		m_Actor->SetPosition(m_World->GetPlayer()->GetPosition());
	}
}

void Gun::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	states.transform.rotate(m_Direction * (180.0f/ 3.1415f));
	target.draw(m_RectShape, states);
}

void Gun::Shoot()
{
	if (m_ReloadingTimeRemaining != sf::Time::Zero) return;

	if (m_BulletsInClip == 0)
	{
		Reload();
		if (Empty()) return;
	}

	const float cosDir = cos(m_Direction);
	const float sinDir = sin(m_Direction);

	//const float screenJoltAmount = 2.0f + (std::rand() % 8);
	//m_World.JoltCamera(screenJoltAmount * cosDir, screenJoltAmount * sinDir);

	const sf::Vector2f posOffset = sf::Vector2f(cosDir * 22.0f, sinDir * 22.0f);
	const sf::Vector2f playerVel = m_World.GetPlayer()->GetPhysicsActor()->GetLinearVelocity();
	Bullet* newBullet = new Bullet(m_World, m_Map, m_Actor->GetPosition() + posOffset, m_Direction, playerVel / 2.0f);
	--m_BulletsInClip;
	ApexAudio::PlaySoundEffect(ApexAudio::Sound::GUN_FIRE);

	if (m_BulletsInClip == 0) Reload();
}

void Gun::Reload()
{
	if (m_BulletsInClip == m_ClipSize) return;

	if (m_BulletsRemaining == 0)
	{
		ApexAudio::PlaySoundEffect(ApexAudio::Sound::GUN_FIRE_EMPTY);
		return;
	}
	else
	{
		ApexAudio::PlaySoundEffect(ApexAudio::Sound::GUN_RELOAD);
		const int diff = m_ClipSize - m_BulletsInClip;
		m_BulletsRemaining -= diff;
		m_BulletsInClip = m_ClipSize;

		m_ReloadingTimeRemaining = sf::seconds(RELOAD_SECONDS);
	}
}
