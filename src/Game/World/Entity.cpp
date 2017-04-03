
#include "Game\World\Entity.h"

#include "Apex\Physics\PhysicsActor.h"
#include "Apex\Graphics\TextureManager.h"

#include <SFML\Graphics\Sprite.hpp>

#include <Box2D\Dynamics\Contacts\b2Contact.h>

Entity::Entity(World& world, Map& map, sf::Vector2f position, ActorID id, void* userPointer, b2BodyType bodyType) :
	m_World(world), m_Map(map),
	m_Position(position), m_UserData(int(id)), m_UserPointer(userPointer), m_BodyType(bodyType)
{
	m_ShadowSprite = sf::Sprite(*apex::TextureManager::GetTexture(Texture::SHADOW));
}

Entity::~Entity()
{
	delete m_Actor;
}

void Entity::CreatePhysicsActor()
{
	if (m_Actor == nullptr)
	{
		m_Actor = new apex::PhysicsActor(m_Position, m_BodyType, 0.0f);
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

apex::PhysicsActor* Entity::GetPhysicsActor() const
{
	return m_Actor;
}

Map& Entity::GetMap() const
{
	return m_Map;
}

void Entity::DrawShadow(sf::RenderTarget & target, sf::RenderStates states)
{
	static const float halfShadowImageWidth = apex::TextureManager::GetTexture(Texture::SHADOW)->getSize().x / 2.0f;
	states.transform.translate(-halfShadowImageWidth, 0);
	target.draw(m_ShadowSprite, states);
}
