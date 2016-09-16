#pragma once

#include "Entity.h"

class Item : public Entity
{
public:
	Item(World& world, Map& map, sf::Vector2f position, ActorID actorID, void* userPointer, b2BodyType bodyType = b2BodyType::b2_dynamicBody);
	virtual ~Item();

	Item(const Item&) = delete;
	Item& operator=(const Item&) = delete;

	virtual void Tick(sf::Time elapsed) override = 0;
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states) override = 0;

private:

};
