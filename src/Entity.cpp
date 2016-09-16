
#include "Entity.h"
#include "PhysicsActor.h"
#include "TextureManager.h"

#include <SFML\Graphics\Sprite.hpp>

#include <Box2D\Dynamics\Contacts\b2Contact.h>

Entity::Entity(World& world, Map& map, sf::Vector2f position, ActorID id, void* userPointer, b2BodyType bodyType) :
	m_World(world), m_Map(map),
	m_Position(position), m_UserData(id), m_UserPointer(userPointer), m_BodyType(bodyType)
{
	m_ShadowSprite = sf::Sprite(*TextureManager::GetTexture(TextureManager::SHADOW));
}

Entity::~Entity()
{
	delete m_Actor;
}

void Entity::CreatePhysicsActor()
{
	if (m_Actor == nullptr)
	{
		m_Actor = new PhysicsActor(m_Position, m_BodyType, 0.0f);
		m_Actor->SetUserData(m_UserData);
		m_Actor->SetUserPointer(m_UserPointer);
	}
}

void Entity::DeletePhysicsActor()
{
	if (m_Actor != nullptr)
	{
		delete m_Actor;
		m_Actor = nullptr;
	}
}

PhysicsActor* Entity::GetPhysicsActor() const
{
	return m_Actor;
}

Map& Entity::GetMap() const
{
	return m_Map;
}
