
#include "Fixture.h"
#include "PhysicsBody.h"

Fixture::Fixture()
{
	m_BoundingRectangle.setFillColor(sf::Color(80, 80, 80, 150));
}

Fixture::~Fixture()
{
}

bool Fixture::IsPointInFixture(sf::Vector2f point) const
{
	return false;
}

void Fixture::Tick(sf::Time elapsed)
{
	m_BoundingRectangle.setPosition(m_Body->GetPosition() + m_RelativePos);
}

void Fixture::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	target.draw(m_BoundingRectangle, states);
}
