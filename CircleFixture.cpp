
#include "CircleFixture.h"
#include "PhysicsActor.h"

CircleFixture::CircleFixture(PhysicsActor* parentActor, float radius, float restitution) :
	Fixture(parentActor, Shape::CIRCLE, restitution),
	m_Radius(radius)
{
	m_BoundingCircle.setFillColor(m_FillColor);
	m_BoundingCircle.setOutlineColor(m_OutlineColor);
	m_BoundingCircle.setOutlineThickness(m_OutlineThickness);
	m_BoundingCircle.setRadius(radius);
	m_RelativePos = sf::Vector2f(-radius, -radius);
	m_BoundingCircle.setPosition(m_ParentActor->GetPosition() + m_RelativePos);
}

CircleFixture::~CircleFixture()
{
}

void CircleFixture::Tick(sf::Time elapsed)
{
	if (m_ParentActor->GetType() != PhysicsActor::Type::STATIC)
	{
		m_BoundingCircle.setPosition(m_ParentActor->GetPosition() + m_RelativePos);
	}
}

void CircleFixture::Draw(sf::RenderTarget & target, sf::RenderStates states)
{
	target.draw(m_BoundingCircle, states);
}

float CircleFixture::GetRadius() const
{
	return m_Radius;
}

bool CircleFixture::IsOverlapping(Fixture* otherFixture)
{
	switch (otherFixture->GetShape())
	{
	case Shape::BOX:
	{
		return false;
	} break;
	case Shape::CIRCLE:
	{
		CircleFixture* otherCircleFixture = (CircleFixture*)otherFixture;
		const sf::Vector2f ourPos = GetPosition();
		const sf::Vector2f otherPos = otherCircleFixture->GetPosition();
		const float dx = (otherPos.x - ourPos.x);
		const float dy = (otherPos.y - ourPos.y);
		return (m_Radius * m_Radius) < (dx*dx + dy*dy);
	} break;
	default:
		return false;
	}
}

bool CircleFixture::IsPointInFixture(sf::Vector2f point) const
{
	const sf::Vector2f ourPos = GetPosition();
	float dx = ourPos.x - point.x;
	float dy = ourPos.y - point.y;
	return (dx * dx + dy * dy) < (m_Radius * m_Radius);
}
