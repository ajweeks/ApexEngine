#include "Entity.h"

Entity::Entity(Type type) : 
	m_Type(type)
{
}

Entity::~Entity()
{
}

sf::Vector2f Entity::GetVelocity() const
{
	return m_Vel;
}

