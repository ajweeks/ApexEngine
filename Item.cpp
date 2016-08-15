
#include "Item.h"


Item::Item(World* world, sf::Vector2f position, ActorID actorID, void* userPointer, b2BodyType bodyType) :
	Entity(world, position, actorID, userPointer, bodyType)
{
}

Item::~Item()
{
}
