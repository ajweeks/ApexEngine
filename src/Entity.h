#pragma once

#include "ApexContactListener.h"
#include "enumerations.h"

#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\System\Time.hpp>
#include <SFML\Graphics\Sprite.hpp>

#include <Box2D\Dynamics\b2Body.h>

class PhysicsActor;
class World;
class Map;

class Entity : public ApexContactListener
{
public:
	Entity(World* world, Map* map, sf::Vector2f position, ActorID id, void* userPointer = nullptr, b2BodyType bodyType = b2BodyType::b2_dynamicBody);
	virtual ~Entity();

	Entity& operator=(const Entity&) = delete;
	Entity(const Entity&) = delete;

	virtual void Tick(sf::Time elapsed) = 0;
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states) = 0;

	virtual void BeginContact(ApexContact* contact) override {};
	virtual void EndContact(ApexContact* contact)  override {};
	virtual void PreSolve(ApexContact* contact, bool& enableContact) override {};

	virtual void CreatePhysicsActor();
	virtual void DeletePhysicsActor();

	PhysicsActor* GetPhysicsActor() const;

	Map* GetMap() const;

protected:
	World* m_World = nullptr;
	Map* m_Map = nullptr;
	PhysicsActor* m_Actor = nullptr;
	sf::Sprite m_ShadowSprite;

private:
	sf::Vector2f m_Position;
	b2BodyType m_BodyType;
	int m_UserData;
	void* m_UserPointer;

};
