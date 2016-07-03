
#include <algorithm>
#include "Manifold.h"
#include "PhysicsActor.h"
#include "ApexMath.h"
#include "Collision.h"
#include "Fixture.h"

Manifold::Manifold(PhysicsActor* A, PhysicsActor* B) :
	A(A), B(B)
{
}

Manifold::~Manifold()
{
}

void Manifold::Solve()
{
	Fixture::Shape shapeA = A->GetFixture()->GetShape();
	Fixture::Shape shapeB = B->GetFixture()->GetShape();
	if (shapeA == Fixture::Shape::BOX && shapeB == Fixture::Shape::CIRCLE)
		Collision::BoxVCircle(this, A, B);
	else if (shapeA == Fixture::Shape::CIRCLE && shapeB == Fixture::Shape::BOX)
		Collision::CircleVBox(this, A, B);
	else if (shapeA == Fixture::Shape::CIRCLE && shapeB == Fixture::Shape::CIRCLE)
		Collision::CircleVCircle(this, A, B);
}

void Manifold::Initialize(float dt)
{
	m_E = std::min(A->GetRestitution(), B->GetRestitution());

	m_StaticFriction = std::sqrt(A->GetStaticFriction() * B->GetStaticFriction());
	m_DynamicFriction = std::sqrt(A->GetDynamicFriction() * B->GetDynamicFriction());

	for (size_t i = 0; i < m_ContactCount; ++i)
	{
		sf::Vector2f ra = m_Contacts[i] - A->GetPosition();
		sf::Vector2f rb = m_Contacts[i] - B->GetPosition();

		sf::Vector2f relativeVel = B->GetVelocity() + ApexMath::Cross(B->GetAngularVelocity(), rb) -
			A->GetVelocity() - ApexMath::Cross(A->GetAngularVelocity(), ra);
	}
}

void Manifold::ApplyImpulse()
{
	if (A->GetInverseMass() + B->GetInverseMass() == 0.0f)
	{
		InfiniteMassCorrection();
		return;
	}

	for (size_t i = 0; i < m_ContactCount; ++i)
	{
		sf::Vector2f ra = m_Contacts[i] - A->GetPosition();
		sf::Vector2f rb = m_Contacts[i] - B->GetPosition();

		sf::Vector2f relativeVel = B->GetVelocity() + ApexMath::Cross(B->GetAngularVelocity(), rb) -
			A->GetVelocity() - ApexMath::Cross(A->GetAngularVelocity(), ra);

		float contactVel = ApexMath::Dot(relativeVel, m_Normal);

		// velocites are already separating
		if (contactVel > 0) return;

		float raCrossN = ApexMath::Cross(ra, m_Normal);
		float rbCrossN = ApexMath::Cross(rb, m_Normal);
		float invMassSum = A->GetInverseMass() + B->GetInverseMass() + 
			(raCrossN * raCrossN) * A->GetInverseInertia() +
			(rbCrossN * rbCrossN) * B->GetInverseInertia();

		float j = -(1.0f + m_E) * contactVel;
		j /= invMassSum;
		j /= (float)m_ContactCount;

		sf::Vector2f impulse = m_Normal * j;
		A->ApplyImpulse(-impulse, ra);
		B->ApplyImpulse(impulse, rb);

		// Is this line required?
		relativeVel = B->GetVelocity() + ApexMath::Cross(B->GetAngularVelocity(), rb) -
			A->GetVelocity() - ApexMath::Cross(A->GetAngularVelocity(), ra);

		sf::Vector2f t = relativeVel - (m_Normal * ApexMath::Dot(relativeVel, m_Normal));
		t = ApexMath::Normalize(t);

		float jt = -ApexMath::Dot(relativeVel, t);
		jt /= invMassSum;
		jt/= (float)m_ContactCount;

		if (jt == 0.0f)
			return;

		sf::Vector2f tangentImpulse;
		if (std::abs(jt) < j * m_StaticFriction)
		{
			tangentImpulse = t * jt;
		}
		else
		{
			tangentImpulse = t * -j * m_DynamicFriction;
		}

		A->ApplyImpulse(-tangentImpulse, ra);
		B->ApplyImpulse(-tangentImpulse, rb);
	}
}

void Manifold::Positionalorrection()
{
	const float k_slop = 0.05f;
	const float percent = 0.4f;
	sf::Vector2f correction = (std::max(m_Penetration - k_slop, 0.0f) / (A->GetInverseMass() + B->GetInverseMass())) * m_Normal * percent;
	A->SetPosition(A->GetPosition() - correction * A->GetInverseMass());
	B->SetPosition(B->GetPosition() + correction * B->GetInverseMass());
}

void Manifold::InfiniteMassCorrection()
{
	A->SetVelocity(sf::Vector2f(0.0f, 0.0f));
	B->SetVelocity(sf::Vector2f(0.0f, 0.0f));
}

void Manifold::SetContactCount(size_t contactCount)
{
	m_ContactCount = contactCount;
}

size_t Manifold::GetContactCount() const
{
	return m_ContactCount;
}

void Manifold::SetPenetration(float penetration)
{
	m_Penetration = penetration;
}

float Manifold::GetPenetration() const
{
	return m_Penetration;
}

void Manifold::SetNormal(sf::Vector2f normal)
{
	m_Normal = normal;
}

sf::Vector2f Manifold::GetNormal() const
{
	return m_Normal;
}

void Manifold::SetContact(int contactIndex, sf::Vector2f position)
{
	m_Contacts[contactIndex] = position;
}
