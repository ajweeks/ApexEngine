
#include "Gun.h"
#include "ApexMain.h"
#include "Bullet.h"
#include "PhysicsActor.h"
#include "Level.h"
#include "ApexMouse.h"
#include "ApexAudio.h"
#include "Player.h"
#include "AmmoDrop.h"

Gun::Gun(Level* level, sf::Vector2f position) :
	Entity(level, position, ActorID::GUN, this),
	m_Level(level)
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
	m_ClipsRemaining = 1;
	m_BulletsRemaining = m_ClipSize;
}

bool Gun::Empty() const
{
	return m_BulletsRemaining == 0 && m_ClipsRemaining == 0;
}

void Gun::AddAmmo(AmmoDrop* ammoDrop)
{
	m_ClipsRemaining += int(ammoDrop->GetBulletCount() / m_ClipSize);
}

int Gun::GetBulletsRemaining() const
{
	return m_BulletsRemaining;
}

int Gun::GetClipSize() const
{
	return m_ClipSize;
}

int Gun::GetClipsRemaining() const
{
	return m_ClipsRemaining;
}

bool Gun::OnButtonPress(sf::Event::MouseButtonEvent buttonEvent)
{
	if (m_Level->IsPaused()) return true;
	if (APEX->DEBUGIsGamePaused()) return true;

	if (buttonEvent.button == sf::Mouse::Button::Left)
	{
		Shoot();
		return false;
	}
	return true;
}

void Gun::OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent)
{
	if (m_Level->IsPaused()) return;
	if (APEX->DEBUGIsGamePaused()) return;
}

void Gun::OnScroll(sf::Event::MouseWheelScrollEvent scrollEvent)
{
	if (m_Level->IsPaused()) return;
	if (APEX->DEBUGIsGamePaused()) return;
}

float Gun::GetDirection() const
{
	return m_Direction;
}

void Gun::Tick(sf::Time elapsed)
{
	if (m_Level->GetPlayer() != nullptr)
	{
		const sf::Vector2f mousePos = static_cast<sf::Vector2f>(APEX->GetMouseCoordsScreenSpace(m_Level->GetCurrentView()));
		if (mousePos.x != -1.0f && mousePos.y != -1.0f) // only update our rotation when the mouse is inside the window
		{
			const sf::Vector2f dPos = mousePos - m_Actor->GetPosition();
			m_Direction = atan2(dPos.y, dPos.x);
		}
		
		m_Actor->SetPosition(m_Level->GetPlayer()->GetPosition());
	}
}

void Gun::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	states.transform.rotate(m_Direction * (180.0f/ 3.1415f));
	target.draw(m_RectShape, states);
}

void Gun::Shoot()
{
	if (m_BulletsRemaining == 0)
	{
		Reload();
		if (Empty()) return;
	}

	const float cosDir = cos(m_Direction);
	const float sinDir = sin(m_Direction);

	//const float screenJoltAmount = 2.0f + (std::rand() % 8);
	//m_Level->JoltCamera(screenJoltAmount * cosDir, screenJoltAmount * sinDir);

	const sf::Vector2f posOffset = sf::Vector2f(cosDir * 22.0f, sinDir * 22.0f);
	const sf::Vector2f playerVel = m_Level->GetPlayer()->GetPhysicsActor()->GetLinearVelocity();
	Bullet* newBullet = new Bullet(m_Level, m_Actor->GetPosition() + posOffset, m_Direction, playerVel / 2.0f);
	--m_BulletsRemaining;
	ApexAudio::PlaySoundEffect(ApexAudio::Sound::GUN_FIRE);

	if (m_BulletsRemaining == 0) Reload();
}

void Gun::Reload()
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
