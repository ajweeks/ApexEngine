
#include "CircleFixture.h"
#include "PhysicsActor.h"
#include "BoxFixture.h"

CircleFixture::CircleFixture(PhysicsActor* parentActor, float radius) :
	Fixture(parentActor, Shape::CIRCLE),
	m_Radius(radius)
{
	m_BoundingCircle.setFillColor(m_FillColor);
	m_BoundingCircle.setOutlineColor(m_OutlineColor);
	m_BoundingCircle.setOutlineThickness(m_OutlineThickness);
	m_BoundingCircle.setRadius(radius);
	m_RelativePos = sf::Vector2f(-radius, -radius);
	m_BoundingCircle.setPosition(m_ParentActor->GetPosition() + m_RelativePos);

	ComputeMass(1.0f);
}

CircleFixture::~CircleFixture()
{
}

void CircleFixture::Tick(sf::Time elapsed)
{
	if (m_ParentActor->GetBodyType() != PhysicsActor::BodyType::STATIC)
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

void CircleFixture::ComputeMass(float density)
{
	const float mass = 3.14159f * m_Radius * density;
	if (mass == 0.0f) m_ParentActor->SetInverseMass(0.0f);
	else m_ParentActor->SetInverseMass(1.0f / mass);

	const float inertia = mass * m_Radius * m_Radius;
	if (inertia == 0.0f) m_ParentActor->SetInverseInertia(0.0f);
	else m_ParentActor->SetInverseInertia(1.0f / inertia);
}

bool CircleFixture::IsPointInFixture(sf::Vector2f point) const
{
	const sf::Vector2f ourPos = GetPosition();
	float dx = ourPos.x - point.x;
	float dy = ourPos.y - point.y;
	return (dx * dx + dy * dy) < (m_Radius * m_Radius);
}
