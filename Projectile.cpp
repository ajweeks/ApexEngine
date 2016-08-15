
#include "Projectile.h"

Projectile::Projectile(World* world, sf::Vector2f position, ActorID id, void* userPointer) : 
	Entity(world, position, id, userPointer)
{
}
Projectile::~Projectile()
{
}
