
#include "AmmoDrop.h"
#include "PhysicsActor.h"
#include "World.h"
#include "Player.h"
#include "ApexMain.h"
#include "Map.h"

const float AmmoDrop::WIDTH = 26.0f;
const float AmmoDrop::HEIGHT = 26.0f;

AmmoDrop::AmmoDrop(World& world, Map& map, sf::Vector2f position, int bulletCount) :
	Item(world, map, position, ActorID::AMMO, this, b2BodyType::b2_staticBody),
	m_BulletCount(bulletCount)
{
	m_Actor->AddBoxFixture(WIDTH, HEIGHT, true);
	m_Actor->AddContactListener(this);

	m_Sprite = sf::Sprite(*TextureManager::GetTexture(TextureManager::AMMO));
}

AmmoDrop::~AmmoDrop()
{
}

int AmmoDrop::GetBulletCount() const
{
	return m_BulletCount;
}

void AmmoDrop::Tick(sf::Time elapsed)
{
	m_Sprite.setPosition(m_Actor->GetPosition() - sf::Vector2f(WIDTH / 2.0f, HEIGHT / 2.0f));
	m_ShadowSprite.setPosition(m_Actor->GetPosition() - sf::Vector2f(WIDTH / 2.0f, 0.0f));
	const float secondsElapsed = APEX->GetTimeElapsed().asSeconds();
	const float height = sin(secondsElapsed * 2.0f + m_Actor->GetPosition().x);
	m_Sprite.move(0.0f, height * 5.0f);

	const sf::Uint8 shadowAlpha = sf::Uint8((height + 1.0f) / 2.0f * 127 + 127);
	m_ShadowSprite.setColor(sf::Color(255, 255, 255, shadowAlpha));
}

void AmmoDrop::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	target.draw(m_ShadowSprite, states);
	target.draw(m_Sprite, states);
}

void AmmoDrop::BeginContact(ApexContact* contact)
{
	switch (contact->actorB->GetUserData())
	{
	case ActorID::PLAYER:
	{
		m_Map.AddEntityToBeRemoved(this);
	} break;
	}
}
