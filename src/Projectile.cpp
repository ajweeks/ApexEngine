
#include "Projectile.h"

Projectile::Projectile(World* world, Map* map, sf::Vector2f position, ActorID id, void* userPointer) : 
	Entity(world, map, position, id, userPointer)
{
}
Projectile::~Projectile()
{
}
