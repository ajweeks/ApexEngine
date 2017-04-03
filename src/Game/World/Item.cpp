
#include "Game\World\Item.h"

#include "Apex\Physics\PhysicsActor.h"

Item::Item(World& world, Map& map, sf::Vector2f position, ActorID actorID, void* userPointer, b2BodyType bodyType) :
	Entity(world, map, position, actorID, userPointer, bodyType)
{
}

Item::~Item()
{
}
