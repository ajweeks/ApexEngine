
#include "Entity.h"
#include "PhysicsActor.h"

#include <Box2D\Dynamics\Contacts\b2Contact.h>

sf::Sprite* Entity::m_ShadowSprite = nullptr;
sf::Texture* Entity::m_ShadowTexture = nullptr;
unsigned int Entity::m_InstanceCount = 0;

Entity::Entity(Level* level, sf::Vector2f position, ActorID id, void* userPointer, b2BodyType bodyType) :
	m_Level(level)
{
	if (++m_InstanceCount == 1)
	{
		m_ShadowTexture = new sf::Texture();
		if (m_ShadowTexture->loadFromFile("resources/shadow.png"))
		{
			m_ShadowSprite = new sf::Sprite(*m_ShadowTexture);
		}
	}

	m_Actor = new PhysicsActor(position, bodyType, 0.0f);
	m_Actor->SetUserData(int(id));
	m_Actor->SetUserPointer(userPointer);
}

Entity::~Entity()
{
	delete m_Actor;

	--m_InstanceCount;
	if (m_InstanceCount == 0)
	{
		delete m_ShadowTexture;
		m_ShadowTexture = nullptr;
		delete m_ShadowSprite;
		m_ShadowSprite = nullptr;
	}
}

PhysicsActor* Entity::GetPhysicsActor() const
{
	return m_Actor;
}
