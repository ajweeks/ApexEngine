
#include "Coin.h"
#include "TextureManager.h"
#include "PhysicsActor.h"
#include "World.h"
#include "Map.h"

Coin::Coin(World& world, Map& map, sf::Vector2f position) :
	Item(world, map, position, ActorID::COIN, this)
{
	m_SpriteSheet.Create(TextureManager::GetTexture(TextureManager::COIN), 16, 16);
	m_SpriteSheet.SetEntireSpriteAsOneSequence(50);
	m_SpriteSheet.SetFrameIndex(int(position.x) % m_SpriteSheet.GetCurrentSequenceFramesLong());
}

Coin::~Coin()
{}

void Coin::Tick(sf::Time elapsed)
{
	m_SpriteSheet.Tick(elapsed);
}

void Coin::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	states.transform.translate(m_Actor->GetPosition());
	m_SpriteSheet.Draw(target, states);
}

sf::Vector2f Coin::GetBottomMiddlePoint()
{
	return m_Actor->GetPosition() + sf::Vector2f(0, 16 / 2.0f);
}

void Coin::BeginContact(ApexContact* contact)
{
	switch (contact->actorB->GetUserData())
	{
	case ActorID::PLAYER:
	{
		m_Map.AddEntityToBeRemoved(this);
	} break;
	}
}

void Coin::CreatePhysicsActor()
{
	Item::CreatePhysicsActor();
	m_Actor->AddBoxFixture(16, 16, true);
	m_Actor->AddContactListener(this);

	b2Filter collisionFilter;
	collisionFilter.categoryBits = ActorID::COIN;
	collisionFilter.maskBits = ActorID::PLAYER;
	m_Actor->SetCollisionFilter(collisionFilter);
}
