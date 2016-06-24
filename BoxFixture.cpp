
#include "BoxFixture.h"
#include "PhysicsActor.h"

BoxFixture::BoxFixture(PhysicsActor* parentActor, sf::Vector2f size, float restitution) :
	Fixture(parentActor, Shape::BOX, restitution),
	m_Size(size)
{
	m_BoundingRectangle.setFillColor(m_FillColor);
	m_BoundingRectangle.setOutlineColor(m_OutlineColor);
	m_BoundingRectangle.setOutlineThickness(m_OutlineThickness);
	m_BoundingRectangle.setSize(size);
	m_RelativePos = sf::Vector2f(-size.x / 2, -size.y / 2);
	m_BoundingRectangle.setPosition(m_ParentActor->GetPosition() + m_RelativePos);
}

BoxFixture::~BoxFixture()
{
}

void BoxFixture::Tick(sf::Time elapsed)
{
	if (m_ParentActor->GetType() != PhysicsActor::Type::STATIC)
	{
		m_BoundingRectangle.setPosition(m_ParentActor->GetPosition() + m_RelativePos);
	}
}

void BoxFixture::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	target.draw(m_BoundingRectangle, states);
}

sf::Vector2f BoxFixture::GetSize() const
{
	return m_Size;
}

bool BoxFixture::IsOverlapping(Fixture* otherFixture)
{
	switch (otherFixture->GetShape())
	{
	case Shape::BOX:
	{
		BoxFixture* otherBoxFixture = (BoxFixture*)otherFixture;
		const sf::Vector2f ourPos = GetPosition();
		const sf::Vector2f otherPos = otherBoxFixture->GetPosition();
		const sf::Vector2f otherSize = otherBoxFixture->GetSize();
		return ourPos.x + m_Size.x > otherPos.x && ourPos.x < otherPos.x + otherSize.x &&
			ourPos.y + m_Size.y > otherPos.y && ourPos.y < otherPos.y + otherSize.y;
	} break;
	case Shape::CIRCLE:
	{

	} break;
	default:
		return false;
	}
	return false;
}

bool BoxFixture::IsPointInFixture(sf::Vector2f point) const
{
	const sf::Vector2f ourPos = m_ParentActor->GetPosition() + m_RelativePos;
	return point.x > ourPos.x &&  point.x < ourPos.x + m_Size.x &&
			point.y > ourPos.y &&  point.y < ourPos.y + m_Size.y;
}