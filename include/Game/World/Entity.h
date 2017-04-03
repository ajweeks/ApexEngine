#pragma once

#include "Game\enumerations.h"

#include "Apex\Physics\ContactListener.h"

#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\System\Time.hpp>
#include <SFML\Graphics\Sprite.hpp>

#include <Box2D\Dynamics\b2Body.h>

class World;
class Map;

namespace apex
{
	class PhysicsActor;
}

class Entity : public apex::ContactListener
{
public:
	Entity(World& world, Map& map, sf::Vector2f position, ActorID id, void* userPointer, b2BodyType bodyType = b2BodyType::b2_dynamicBody);
	virtual ~Entity();

	Entity& operator=(const Entity&) = delete;
	Entity(const Entity&) = delete;

	virtual void Tick(sf::Time elapsed) = 0;
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states) = 0;

	virtual void BeginContact(apex::Contact* contact) override {};
	virtual void EndContact(apex::Contact* contact)  override {};
	virtual void PreSolve(apex::Contact* contact, bool& enableContact) override {};

	virtual void CreatePhysicsActor();
	virtual void DeletePhysicsActor();

	virtual sf::Vector2f GetBottomMiddlePoint() = 0;

	apex::PhysicsActor* GetPhysicsActor() const;

	Map& GetMap() const;

protected:
	void DrawShadow(sf::RenderTarget& target, sf::RenderStates states);

	World& m_World;
	Map& m_Map;

	apex::PhysicsActor* m_Actor = nullptr;
	sf::Sprite m_ShadowSprite;
	sf::Vector2f m_Position;

private:
	b2BodyType m_BodyType;
	int m_UserData;
	void* m_UserPointer;

};
