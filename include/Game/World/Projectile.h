#pragma once

#include "Entity.h"

#include <SFML\System\Time.hpp>

class Projectile : public Entity
{
public:
	Projectile(World& world, Map& map, sf::Vector2f position, ActorID id, void* userPointer);
	virtual ~Projectile();

	Projectile(const Projectile&) = delete;
	Projectile& operator=(const Projectile&) = delete;

	virtual void Tick(sf::Time elapsed) = 0;
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states) = 0;

private:

};
