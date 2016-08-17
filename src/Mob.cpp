
#include "Mob.h"

Mob::Mob(World* world, Map* map, sf::Vector2f position, ActorID id) :
	Entity(world, map, position, id, this)
{
}

Mob::~Mob()
{
}
