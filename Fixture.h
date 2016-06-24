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

	Fixture(PhysicsActor* parentActor, Shape shape, float restitution = 0.8f);
	virtual ~Fixture();

	Fixture(const Fixture&) = delete;
	Fixture& operator=(const Fixture&) = delete;

	virtual void Tick(sf::Time elapsed) = 0;
	virtual bool IsPointInFixture(sf::Vector2f point) const = 0;
	// Called while debug overlay is showing
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states) = 0;
	virtual bool IsOverlapping(Fixture* otherFixture) = 0;

	float GetRestitution() const;
	sf::Vector2f GetPosition() const;
	Shape GetShape() const;

protected:
	sf::Vector2f m_RelativePos; // Relative to our body
	PhysicsActor* m_ParentActor = nullptr;
	
	sf::Color m_FillColor;
	sf::Color m_OutlineColor;
	float m_OutlineThickness;

	Shape m_Shape;
	float m_Restitution;

	// Friction?
	// Restitution?

};
