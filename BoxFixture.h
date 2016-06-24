#pragma once

#include "Fixture.h"

class BoxFixture : public Fixture
{
public:
	BoxFixture(PhysicsActor* parentActor, sf::Vector2f size = sf::Vector2f(32, 32), float restitution = 0.8f);
	virtual ~BoxFixture();

	BoxFixture(const BoxFixture&) = delete;
	BoxFixture& operator=(const BoxFixture&) = delete;

	virtual void Tick(sf::Time elapsed);
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states);

	virtual bool IsPointInFixture(sf::Vector2f point) const;
	sf::Vector2f GetSize() const;
	virtual bool IsOverlapping(Fixture* otherFixture);

private:
	sf::Vector2f m_Size;
	sf::RectangleShape m_BoundingRectangle;

};
