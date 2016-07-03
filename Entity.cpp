
#include "Entity.h"
#include "PhysicsActor.h"

Entity::Entity(Level* level, sf::Vector2f position, Type type, void* userPointer)
{
	m_Actor = new PhysicsActor(position, PhysicsActor::BodyType::DYNAMIC, level);
	m_Actor->SetUserData(int(type));
	m_Actor->SetUserPointer(userPointer);
}

Entity::~Entity()
{
	delete m_Actor;
}

sf::Vector2f Entity::GetPosition() const
{
	return m_Actor->GetPosition();
}

sf::Vector2f Entity::GetVelocity() const
{
	return m_Actor->GetVelocity();
}

