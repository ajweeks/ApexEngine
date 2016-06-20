#pragma once

#include <SFML\Graphics.hpp>

class PhysicsBody;

class Fixture
{
public:
	enum class Shape
	{
		BOX, CIRCLE, POLYGON
	};

	Fixture();
	virtual ~Fixture();

	Fixture(const Fixture&) = delete;
	Fixture& operator=(const Fixture&) = delete;

	void Tick(sf::Time elapsed);
	bool IsPointInFixture(sf::Vector2f point) const;
	// Called while debug overlay is showing
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

private:
	Shape m_Shape;
	sf::RectangleShape m_BoundingRectangle;
	sf::Vector2f m_RelativePos; // Relative to our body
	PhysicsBody* m_Body = nullptr;
	
	// Friction?
	// Restitution?

};
