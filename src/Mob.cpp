
#include "Mob.h"
#include "PhysicsActor.h"

Mob::Mob(World& world, Map& map, sf::Vector2f position, ActorID id, void* userPointer) :
	Entity(world, map, position, id, userPointer)
{
}

Mob::~Mob()
{
}

void Mob::CreatePhysicsActor()
{
	if (m_Actor == nullptr)
	{
		Entity::CreatePhysicsActor();
	}
}

