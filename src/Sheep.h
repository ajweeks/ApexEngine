#pragma once

#include "Mob.h"
#include "ColorTransition.h"
#include "TransformableTransition.h"

#include <SFML\Graphics\RectangleShape.hpp>

class Sheep : public Mob
{
public:
	Sheep(World* world, Map* map, sf::Vector2f position);
	virtual ~Sheep();

	Sheep(const Sheep&) = delete;
	Sheep& operator=(const Sheep&) = delete;

	virtual void Tick(sf::Time elapsed);
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states);

	virtual void BeginContact(PhysicsActor* thisActor, PhysicsActor* otherActor) override;
	virtual void EndContact(PhysicsActor* thisActor, PhysicsActor* otherActor) override;
	virtual void PreSolve(PhysicsActor* thisActor, PhysicsActor* otherActor, bool& enableContact) override;

private:
	static const float WIDTH;
	static const float HEIGHT;

	sf::RectangleShape m_BgRect;
	bool m_NeedToPickNewTarget = false;
	sf::Vector2f m_Vel;
	float m_ForwardVel = 3500.0f;

	int m_HitPoints = 5;
	bool m_IsDead = false;
	ColorTransition m_HurtTransition;
	TransformableTransition m_BlinkTransition;
	float m_SecondsUntilNextBlink;

};
