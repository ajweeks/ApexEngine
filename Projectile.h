#pragma once

#include <SFML\System\Time.hpp>
#include "Entity.h"

class Projectile : public Entity
{
public:
	Projectile(Level* level, sf::Vector2f position, Type type, void* userPointer);
	virtual ~Projectile();

	Projectile(const Projectile&) = delete;
	Projectile& operator=(const Projectile&) = delete;

	virtual void Tick(sf::Time elapsed) = 0;
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states) = 0;

private:

};
