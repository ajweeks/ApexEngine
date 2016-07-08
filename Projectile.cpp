
#include "Projectile.h"

Projectile::Projectile(Level* level, sf::Vector2f position, ActorID id, void* userPointer) : 
	Entity(level, position, id, userPointer)
{
}
Projectile::~Projectile()
{
}
