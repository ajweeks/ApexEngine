
#include "Entity.h"
#include "PhysicsActor.h"
#include "TextureManager.h"

#include <SFML\Graphics\Sprite.hpp>

#include <Box2D\Dynamics\Contacts\b2Contact.h>

Entity::Entity(World* world, sf::Vector2f position, ActorID id, void* userPointer, b2BodyType bodyType) :
	m_World(world)
{
	m_ShadowSprite = sf::Sprite(*TextureManager::GetTexture(TextureManager::SHADOW));

	m_Actor = new PhysicsActor(position, bodyType, 0.0f);
	m_Actor->SetUserData(int(id));
	m_Actor->SetUserPointer(userPointer);
}

Entity::~Entity()
{
	delete m_Actor;
}

PhysicsActor* Entity::GetPhysicsActor() const
{
	return m_Actor;
}
