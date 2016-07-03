#pragma once

#include <SFML\Graphics.hpp>

class PhysicsActor;

class Fixture
{
public:
	enum class Shape
	{
		BOX, CIRCLE, POLYGON
	};

	Fixture(PhysicsActor* parentActor, Shape shape);
	virtual ~Fixture();

	Fixture(const Fixture&) = delete;
	Fixture& operator=(const Fixture&) = delete;

	virtual void Tick(sf::Time elapsed) = 0;
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states) = 0;

	virtual bool IsPointInFixture(sf::Vector2f point) const = 0;
	Shape GetShape() const;
	sf::Vector2f GetPosition() const;

protected:
	sf::Vector2f m_RelativePos; // Relative to our body
	PhysicsActor* m_ParentActor = nullptr;

	sf::Color m_FillColor;
	sf::Color m_OutlineColor;
	float m_OutlineThickness;

	Shape m_Shape;
};
