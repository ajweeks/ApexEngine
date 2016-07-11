
#include "LevelTile.h"


LevelTile::LevelTile(Level* level, sf::Vector2f position, ActorID id) :
	Entity(level, position, id, this, b2BodyType::b2_staticBody)
{
}

LevelTile::~LevelTile()
{
}

void LevelTile::Tick(sf::Time elapsed)
{
}

void LevelTile::Draw(sf::RenderTarget & target, sf::RenderStates states)
{
}

void LevelTile::BeginContact(PhysicsActor * thisActor, PhysicsActor * otherActor)
{
}

void LevelTile::EndContact(PhysicsActor * thisActor, PhysicsActor * otherActor)
{
}

void LevelTile::PreSolve(PhysicsActor * thisActor, PhysicsActor * otherActor, bool & enableContact)
{
}
