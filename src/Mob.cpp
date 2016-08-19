
#include "Mob.h"
#include "PhysicsActor.h"

Mob::Mob(World* world, Map* map, sf::Vector2f position, ActorID id) :
	Entity(world, map, position, id, this)
{
}

Mob::~Mob()
{
}

void Mob::CreatePhysicsActor(ApexContactListener* contactListener)
{
	if (m_Actor == nullptr)
	{
		Entity::CreatePhysicsActor();
		m_Actor->AddContactListener(contactListener);
	}
}

