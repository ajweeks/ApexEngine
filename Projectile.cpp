
#include "Projectile.h"

Projectile::Projectile(Level* level, sf::Vector2f position, Type type, void* userPointer) : 
	Entity(level, position, type, userPointer)
{
}
Projectile::~Projectile()
{
}
