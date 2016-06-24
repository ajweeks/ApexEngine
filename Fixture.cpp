
#include "Fixture.h"
#include "PhysicsActor.h"

Fixture::Fixture(PhysicsActor* parentActor, Shape shape, float restitution) :
	m_ParentActor(parentActor),
	m_Shape(shape), m_Restitution(restitution)
{
	m_FillColor = sf::Color(100, 100, 100, 155);
	m_OutlineColor = sf::Color(150, 150, 150, 255);
	m_OutlineThickness = -1.0f;
}

Fixture::~Fixture()
{
}

float Fixture::GetRestitution() const
{
	return m_Restitution;
}

sf::Vector2f Fixture::GetPosition() const
{
	return m_ParentActor->GetPosition() + m_RelativePos;
}

Fixture::Shape Fixture::GetShape() const
{
	return m_Shape;
}
