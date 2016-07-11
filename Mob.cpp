
#include "Mob.h"


Mob::Mob(Level* level, sf::Vector2f position, ActorID id) :
	Entity(level, position, id, this)
{
}

Mob::~Mob()
{
}
