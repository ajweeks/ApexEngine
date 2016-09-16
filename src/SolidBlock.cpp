
#include "SolidBlock.h"
#include "Map.h"
#include "PhysicsActor.h"

SolidBlock::SolidBlock(World& world, Map& map, sf::Vector2f position) :
	Entity(world, map, position + sf::Vector2f(map.GetTileSize() / 2.0f, map.GetTileSize() / 2.0f), ActorID::SOLID_BLOCK, this, b2BodyType::b2_staticBody)
{
}

SolidBlock::~SolidBlock()
{
}

void SolidBlock::Tick(sf::Time elapsed)
{
}

void SolidBlock::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
}

void SolidBlock::CreatePhysicsActor()
{
	Entity::CreatePhysicsActor();
	const int tileSize = m_Map.GetTileSize();
	m_Actor->AddBoxFixture(float(tileSize), float(tileSize));

	b2Filter collisionFilter;
	collisionFilter.categoryBits = ActorID::SOLID_BLOCK;
	collisionFilter.maskBits = ActorID::PLAYER | ActorID::NPC;
	m_Actor->SetCollisionFilter(collisionFilter);
}

sf::Vector2f SolidBlock::GetBottomMiddlePoint()
{
	const int tileSize = m_Map.GetTileSize();
	return m_Actor->GetPosition() + sf::Vector2f(0, tileSize / 2.0f);
}
