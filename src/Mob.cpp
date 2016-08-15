
#include "Mob.h"


Mob::Mob(World* world, sf::Vector2f position, ActorID id) :
	Entity(world, position, id, this)
{
}

Mob::~Mob()
{
}
