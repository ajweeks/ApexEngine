
#include "Game\World\Coin.h"
#include "Game\World\World.h"
#include "Game\World\Map.h"

#include "Apex\Physics\PhysicsActor.h"
#include "Apex\Graphics\TextureManager.h"

Coin::Coin(World& world, Map& map, sf::Vector2f position) :
	Item(world, map, position, ActorID::COIN, this)
{
	m_SpriteSheet.Create(apex::TextureManager::GetTexture(Texture::COIN), 16, 16);
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
	DrawShadow(target, states);

	states.transform.translate(m_Actor->GetPosition());
	m_SpriteSheet.Draw(target, states);
}

void Coin::DrawShadow(sf::RenderTarget & target, sf::RenderStates states)
{
	states.transform.translate(0.0f, 8.0f);
	states.transform.scale(2.5f, 2.5f);
	Entity::DrawShadow(target, states);
}
sf::Vector2f Coin::GetBottomMiddlePoint()
{
	return m_Actor->GetPosition() + sf::Vector2f(0, 16 / 2.0f);
}

void Coin::BeginContact(apex::Contact* contact)
{
	switch (ActorID(contact->actorB->GetUserData()))
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
	collisionFilter.categoryBits = int(ActorID::COIN);
	collisionFilter.maskBits = int(ActorID::PLAYER);
	m_Actor->SetCollisionFilter(collisionFilter);
}

