#pragma once

#include "Mob.h"

#include "Apex\Transition\ColorTransition.h"
#include "Apex\Transition\TransformableTransition.h"

#include <SFML\Graphics\RectangleShape.hpp>

class Sheep : public Mob
{
public:
	Sheep(World& world, Map& map, sf::Vector2f position);
	virtual ~Sheep();

	Sheep(const Sheep&) = delete;
	Sheep& operator=(const Sheep&) = delete;

	virtual void Tick(sf::Time elapsed);
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states);

	virtual void BeginContact(apex::Contact* contact) override;
	virtual void EndContact(apex::Contact* contact) override;
	virtual void PreSolve(apex::Contact* contact, bool& enableContact) override;

private:
	static const float WIDTH;
	static const float HEIGHT;

	sf::RectangleShape m_BgRect;
	bool m_NeedToPickNewTarget = false;
	sf::Vector2f m_Vel;
	float m_ForwardVel = 3500.0f;

	int m_HitPoints = 5;
	bool m_IsDead = false;
	apex::ColorTransition m_HurtTransition;
	apex::TransformableTransition m_BlinkTransition;
	float m_SecondsUntilNextBlink;

};
