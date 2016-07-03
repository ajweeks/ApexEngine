#pragma once

#include "Fixture.h"
#include <SFML\Graphics\CircleShape.hpp>

class CircleFixture : public Fixture
{
public:
	CircleFixture(PhysicsActor* parentActor, float radius);
	virtual ~CircleFixture();

	CircleFixture(const CircleFixture&) = delete;
	CircleFixture& operator=(const CircleFixture&) = delete;

	virtual void Tick(sf::Time elapsed);
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states);

	virtual bool IsPointInFixture(sf::Vector2f point) const;
	float GetRadius() const;

private:
	void ComputeMass(float density);

	float m_Radius;
	sf::CircleShape m_BoundingCircle;

};
