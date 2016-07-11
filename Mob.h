#pragma once

#include "Entity.h"

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\RenderStates.hpp>

class Mob : public Entity
{
public:
	Mob(Level* level, sf::Vector2f position, ActorID id);
	virtual ~Mob();

	Mob(const Mob&) = delete;
	Mob& operator=(const Mob&) = delete;

	virtual void Tick(sf::Time elapsed) = 0;
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states) = 0;
	
	virtual void BeginContact(PhysicsActor* thisActor, PhysicsActor* otherActor) override {};

private:

};
