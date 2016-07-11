
#include "Sheep.h"
#include "PhysicsActor.h"
#include "ApexMain.h"
#include "Level.h"
#include "ApexMath.h"

Sheep::Sheep(Level* level, sf::Vector2f position) :
	Mob(level, position, ActorID::SHEEP)
{
	m_Actor->AddBoxFixture(15, 15);
	m_Actor->AddContactListener(this);

	m_BgRect = sf::RectangleShape(sf::Vector2f(15.0f, 15.0f));
	m_BgRect.setPosition(m_Actor->GetPosition() - m_BgRect.getSize() / 2.0f);
	m_Actor->SetAngle(-PI / 2.0f);
	m_Vel = sf::Vector2f(cos(PI_2 + m_Actor->GetAngle()) * m_ForwardVel, sin(PI_2 + m_Actor->GetAngle()) * m_ForwardVel);
	m_Actor->SetLinearVelocity(m_Vel);

	TransitionData hurtTransitionStart;
	hurtTransitionStart.color = sf::Color(200, 110, 80);
	TransitionData hurtTransitionEnd;
	hurtTransitionEnd.color = sf::Color(235, 240, 250);
	m_HurtTransition = ApexTransition(hurtTransitionStart, hurtTransitionEnd, sf::milliseconds(450), ApexTransition::EaseType::LINEAR);
	m_HurtTransition.SetFinished();
}

Sheep::~Sheep()
{
}

void Sheep::Tick(sf::Time elapsed)
{
	if (m_IsDead)
	{
		m_Level->RemoveMob(this);
		return;
	}

	if (m_NeedToPickNewTarget)
	{
		m_NeedToPickNewTarget = false;
		const float newAngle = m_Actor->GetAngle() + (std::rand() % 180) / 180.0f * PI + PI_2;
		m_Actor->SetAngle(newAngle);
		m_Vel = sf::Vector2f(cos(PI_2 + newAngle) * m_ForwardVel, sin(PI_2 + newAngle) * m_ForwardVel);
	}

	m_HurtTransition.Tick(elapsed);

	m_Actor->SetLinearVelocity(m_Vel);
	m_BgRect.setPosition(m_Actor->GetPosition() - m_BgRect.getSize() / 2.0f);
}

void Sheep::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	const TransitionData hurtTransitionData = m_HurtTransition.GetCurrentTransitionData();

	const sf::Color pColor = m_BgRect.getFillColor();
	
	const sf::Vector2f center = m_BgRect.getPosition() + m_BgRect.getSize() / 2.0f;

	states.transform.rotate(m_Actor->GetAngle() * 180.0f / PI, center);
	m_BgRect.setFillColor(pColor * hurtTransitionData.color);
	target.draw(m_BgRect, states);

	m_BgRect.setFillColor(sf::Color::Black);
	states.transform.scale(sf::Vector2f(0.2f, 0.2f), center);
	float x = 15.0f;
	float y = 10.0f;
	states.transform.translate(x, y);
	target.draw(m_BgRect, states);
	states.transform.translate(-2 * x, 0.0f);
	target.draw(m_BgRect, states);

	m_BgRect.setFillColor(pColor);
}

void Sheep::BeginContact(PhysicsActor* thisActor, PhysicsActor* otherActor)
{
	switch (otherActor->GetUserData())
	{
	case int(ActorID::WALL):
	{
		m_NeedToPickNewTarget = true;
	} break;
	case int(ActorID::BULLET):
	{
		--m_HitPoints;
		if (m_HitPoints == 0)
		{
			m_IsDead = true;
		}
		else
		{
			m_HurtTransition.Restart();
		}
	} break;
	}
}

void Sheep::EndContact(PhysicsActor* thisActor, PhysicsActor* otherActor)
{
}

void Sheep::PreSolve(PhysicsActor* thisActor, PhysicsActor* otherActor, bool& enableContact)
{
}
