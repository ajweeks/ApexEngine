
#include "Sheep.h"
#include "PhysicsActor.h"
#include "ApexMain.h"
#include "Level.h"
#include "ApexMath.h"
#include "DustParticle.h"

const float Sheep::WIDTH = 15.0f;
const float Sheep::HEIGHT = 15.0f;

Sheep::Sheep(Level* level, sf::Vector2f position) :
	Mob(level, position, ActorID::SHEEP)
{
	m_Actor->AddBoxFixture(15, 15);
	m_Actor->AddContactListener(this);

	b2Filter collisionFilter;
	collisionFilter.categoryBits = ActorID::SHEEP;
	collisionFilter.maskBits = ActorID::BULLET | ActorID::WALL | ActorID::PLAYER;
	m_Actor->SetCollisionFilter(collisionFilter);

	m_BgRect = sf::RectangleShape(sf::Vector2f(WIDTH, HEIGHT));
	m_BgRect.setPosition(m_Actor->GetPosition() - m_BgRect.getSize() / 2.0f);
	m_Actor->SetAngle(-PI / 2.0f);
	m_Vel = sf::Vector2f(cos(PI_2 + m_Actor->GetAngle()) * m_ForwardVel, sin(PI_2 + m_Actor->GetAngle()) * m_ForwardVel);
	m_Actor->SetLinearVelocity(m_Vel);

	TransitionData hurtTransitionStart;
	hurtTransitionStart.color = sf::Color(200, 110, 80);
	TransitionData hurtTransitionEnd;
	hurtTransitionEnd.color = sf::Color(235, 240, 250);
	m_HurtTransition = TransformationTransition(hurtTransitionStart, hurtTransitionEnd, sf::milliseconds(450), ApexTransition::EaseType::LINEAR);
	m_HurtTransition.SetFinished();

	TransitionData blinkTransitionStart;
	blinkTransitionStart.transformable.scale(1.0f, 0.1f);
	TransitionData blinkTransitionEnd;
	m_BlinkTransition = TransformationTransition(blinkTransitionStart, blinkTransitionEnd, sf::milliseconds(80), ApexTransition::EaseType::LINEAR);
	m_SecondsUntilNextBlink = 2.0f + rand() % 3;
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

	m_BlinkTransition.Tick(elapsed);
	m_SecondsUntilNextBlink -= elapsed.asSeconds();
	if (m_SecondsUntilNextBlink <= 0.0f)
	{
		m_SecondsUntilNextBlink = rand() % 3 + 4.0f;
		m_BlinkTransition.Restart();
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

	// Body
	float angle = m_Actor->GetAngle() * 180.0f / PI;
	states.transform.rotate(angle, center);
	m_BgRect.setFillColor(pColor * hurtTransitionData.color);
	target.draw(m_BgRect, states);

	// Eyes
	const TransitionData blinkTransitionData = m_BlinkTransition.GetCurrentTransitionData();
	const sf::Vector2f blinkScale = blinkTransitionData.transformable.getScale();
	states.transform.scale(blinkScale, center + sf::Vector2f(0.0f, 3.5f));

	m_BgRect.setFillColor(sf::Color::Black);
	states.transform.scale(sf::Vector2f(0.16f, 0.16f), center);
	float x = 15.0f;
	float y = 24.0f;
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

			DustParticle *dustParticle = new DustParticle(m_Actor->GetPosition());
			m_Level->AddParticle(dustParticle);
		}
		else
		{
			m_HurtTransition.Restart();
			if (rand() % 8 > 5) m_BlinkTransition.Restart();
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
