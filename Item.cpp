
#include "Item.h"


Item::Item(Level* level, sf::Vector2f position, ActorID actorID, void* userPointer, b2BodyType bodyType) :
	Entity(level, position, actorID, userPointer, bodyType)
{
}

Item::~Item()
{
}
