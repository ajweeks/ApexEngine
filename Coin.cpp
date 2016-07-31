
#include "Coin.h"
#include "TextureManager.h"
#include "PhysicsActor.h"
#include "Level.h"

Coin::Coin(Level * level, sf::Vector2f position) :
	Item(level, position, ActorID::COIN, this)
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
		m_Level->AddItemToBeRemoved(this);
	} break;
	}
}
