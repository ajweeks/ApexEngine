#pragma once

#include "Entity.h"

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

class Mob : public Entity
{
public:
	Mob(World& world, Map& map, sf::Vector2f position, ActorID id, void* userPointer);
	virtual ~Mob();

	Mob(const Mob&) = delete;
	Mob& operator=(const Mob&) = delete;

	virtual void Tick(sf::Time elapsed) = 0;
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states) = 0;

	virtual void BeginContact(apex::Contact* contact) override {};

	virtual void CreatePhysicsActor() override;

private:

};
