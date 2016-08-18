
#include "Coin.h"
#include "TextureManager.h"
#include "PhysicsActor.h"
#include "World.h"
#include "Map.h"

Coin::Coin(World* world, Map* map, sf::Vector2f position) :
	Item(world, map, position, ActorID::COIN, this)
{
	m_Actor->AddBoxFixture(16, 16);
	m_Actor->SetSensor(true);
	m_Actor->AddContactListener(this);

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

void Coin::BeginContact(PhysicsActor* thisActor, PhysicsActor* otherActor)
{
	switch (otherActor->GetUserData())
	{
	case ActorID::PLAYER:
	{
		m_Map->AddItemToBeRemoved(this);
	} break;
	}
}