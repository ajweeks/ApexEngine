#pragma once

#include "Fixture.h"
#include <SFML\Graphics\RectangleShape.hpp>

class BoxFixture : public Fixture
{
public:
	BoxFixture(PhysicsActor* parentActor, sf::Vector2f size = sf::Vector2f(32, 32));
	virtual ~BoxFixture();

	BoxFixture(const BoxFixture&) = delete;
	BoxFixture& operator=(const BoxFixture&) = delete;

	virtual void Tick(sf::Time elapsed);
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states);

	virtual bool IsPointInFixture(sf::Vector2f point) const;
	sf::Vector2f GetSize() const;

	size_t GetVertexCount() const;
	std::vector<sf::Vector2f> GetNormals() const;
	std::vector<sf::Vector2f> GetVerticies() const;

private:
	void ComputeMass(float density);

	sf::Vector2f m_Size;
	sf::RectangleShape m_BoundingRectangle;

	size_t m_VertexCount;
	std::vector<sf::Vector2f> m_Verticies;
	std::vector<sf::Vector2f> m_Normals;
};
