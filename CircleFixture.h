#pragma once

#include "Fixture.h"

class CircleFixture : public Fixture
{
public:
	CircleFixture(PhysicsActor* parentActor, float radius, float restitution = 0.8f);
	virtual ~CircleFixture();

	CircleFixture(const CircleFixture&) = delete;
	CircleFixture& operator=(const CircleFixture&) = delete;

	virtual void Tick(sf::Time elapsed);
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states);

	virtual bool IsPointInFixture(sf::Vector2f point) const;
	float GetRadius() const;
	virtual bool IsOverlapping(Fixture* otherFixture);

private:
	float m_Radius;
	sf::CircleShape m_BoundingCircle;

};
