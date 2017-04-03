
#include "Game\World\Sheep.h"
#include "Game\World\World.h"
#include "Game\World\Map.h"
#include "Game\DustParticle.h"
#include "Game\Game.h"

#include "Apex\Physics\PhysicsActor.h"
#include "Apex\ApexMath.h"
#include "Apex\Physics\Contact.h"

const float Sheep::WIDTH = 15.0f;
const float Sheep::HEIGHT = 15.0f;

Sheep::Sheep(World& world, Map& map, sf::Vector2f position) :
	Mob(world, map, position, ActorID::SHEEP, this)
{
	m_Actor->AddBoxFixture(15, 15);
	m_Actor->AddContactListener(this);

	b2Filter collisionFilter;
	collisionFilter.categoryBits = int(ActorID::SHEEP);
	collisionFilter.maskBits = int(ActorID::BULLET) | int(ActorID::BUILDING) | int(ActorID::PLAYER);
	m_Actor->SetCollisionFilter(collisionFilter);

	m_BgRect = sf::RectangleShape(sf::Vector2f(WIDTH, HEIGHT));
	m_BgRect.setPosition(m_Actor->GetPosition() - m_BgRect.getSize() / 2.0f);
	m_Actor->SetAngle(-b2_pi / 2.0f);
	m_Vel = sf::Vector2f(cos(b2_pi * 2.0f + m_Actor->GetAngle()) * m_ForwardVel, sin(b2_pi * 2.0f + m_Actor->GetAngle()) * m_ForwardVel);
	m_Actor->SetLinearVelocity(m_Vel);

	sf::Color hurtTransitionStart;
	hurtTransitionStart = sf::Color(200, 110, 80);
	sf::Color hurtTransitionEnd;
	hurtTransitionEnd = sf::Color(235, 240, 250);
	m_HurtTransition = apex::ColorTransition(hurtTransitionStart, hurtTransitionEnd, sf::milliseconds(450), apex::Transition::EaseType::LINEAR);
	m_HurtTransition.SetFinished();

	sf::Transformable blinkTransitionStart;
	blinkTransitionStart.scale(1.0f, 0.1f);
	sf::Transformable blinkTransitionEnd;
	m_BlinkTransition = apex::TransformableTransition(blinkTransitionStart, blinkTransitionEnd, sf::milliseconds(80), apex::Transition::EaseType::LINEAR);
	m_SecondsUntilNextBlink = 2.0f + rand() % 3;
}

Sheep::~Sheep()
{
}

void Sheep::Tick(sf::Time elapsed)
{
	if (m_IsDead)
	{
		m_Map.RemoveEntity(this);
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
		const float newAngle = m_Actor->GetAngle() + (std::rand() % 180) / 180.0f * b2_pi + b2_pi * 2.0f;
		m_Actor->SetAngle(newAngle);
		m_Vel = sf::Vector2f(cos(b2_pi * 2.0f + newAngle) * m_ForwardVel, sin(b2_pi * 2.0f + newAngle) * m_ForwardVel);
	}

	m_HurtTransition.Tick(elapsed);

	m_Actor->SetLinearVelocity(m_Vel);
	m_BgRect.setPosition(m_Actor->GetPosition() - m_BgRect.getSize() / 2.0f);
}

void Sheep::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	const sf::Color hurtTransitionColor = m_HurtTransition.GetCurrentColor();
	const sf::Color pColor = m_BgRect.getFillColor();
	const sf::Vector2f center = m_BgRect.getPosition() + m_BgRect.getSize() / 2.0f;

	// Body
	float angle = m_Actor->GetAngle() * 180.0f / b2_pi;
	states.transform.rotate(angle, center);
	m_BgRect.setFillColor(pColor * hurtTransitionColor);
	target.draw(m_BgRect, states);

	// Eyes
	const sf::Transformable blinkTransitionData = m_BlinkTransition.GetCurrentTransformable();
	const sf::Vector2f blinkScale = blinkTransitionData.getScale();
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

void Sheep::BeginContact(apex::Contact* contact)
{
	switch (contact->actorB->GetUserData())
	{
	case int(ActorID::BUILDING):
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
			m_World.AddParticle(dustParticle);
		}
		else
		{
			m_HurtTransition.Restart();
			if (rand() % 8 > 5) m_BlinkTransition.Restart();
		}
	} break;
	}
}

void Sheep::EndContact(apex::Contact* contact)
{
}

void Sheep::PreSolve(apex::Contact* contact, bool& enableContact)
{
}
