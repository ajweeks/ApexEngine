
#include "Apex\Physics\PhysicsActorManager.h"
#include "Apex\Graphics\DebugDraw.h"
#include "Apex\ApexMain.h"

#include <Box2D\Common\b2Math.h>
#include <Box2D\Collision\Shapes\b2PolygonShape.h>
#include <Box2D\Dynamics\b2Fixture.h>
#include <Box2D\Common\b2Draw.h>

namespace apex
{
	const int PhysicsActorManager::VEL_ITERATIONS = 8;
	const int PhysicsActorManager::POS_ITERATIONS = 3;
	const float PhysicsActorManager::TIMESTEP = 1.0f / 60.0f;

	PhysicsActorManager::PhysicsActorManager(sf::RenderTarget& target)
	{
		m_World = new b2World(b2Vec2(0.0f, 0.0f));
		m_World->SetContactListener(APEX);

		m_DebugDraw = new DebugDraw(target);
		m_DebugDraw->AppendFlags(b2Draw::e_shapeBit | b2Draw::e_centerOfMassBit);
		m_World->SetDebugDraw(m_DebugDraw);
	}

	PhysicsActorManager::~PhysicsActorManager()
	{
		delete m_World;
		delete m_DebugDraw;
	}

	void PhysicsActorManager::Tick(sf::Time elapsed)
	{
		m_World->Step(elapsed.asSeconds(), VEL_ITERATIONS, POS_ITERATIONS);
	}

	void PhysicsActorManager::Draw(sf::RenderTarget& target)
	{
		m_World->DrawDebugData();
	}

	b2World& PhysicsActorManager::GetWorld() const
	{
		return *m_World;
	}
} // namespace apex