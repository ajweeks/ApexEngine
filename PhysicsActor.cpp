
#include "PhysicsActor.h"
#include "Level.h"
#include "PhysicsActorManager.h"
#include "Fixture.h"

PhysicsActor::PhysicsActor(sf::Vector2f centerPos, Type type, Level* level) :
	m_Level(level), m_Type(type)
{
	m_Level->GetActorManager()->AddPhysicsActor(this);
	m_Pos = centerPos;
	m_Solid = true;

	m_DebugXAxisRect = sf::RectangleShape(sf::Vector2f(15, 2));
	m_DebugXAxisRect.setFillColor(sf::Color::Red);
	m_DebugXAxisRect.setPosition(m_Pos);
	m_DebugYAxisRect = sf::RectangleShape(sf::Vector2f(2, 15));
	m_DebugYAxisRect.setFillColor(sf::Color::Green);
	m_DebugYAxisRect.setPosition(m_Pos);
}

PhysicsActor::~PhysicsActor()
{
	m_Level->GetActorManager()->RemovePhysicsActor(this);
	for (size_t i = 0; i < m_Fixtures.size(); ++i)
	{
		delete m_Fixtures[i];
	}
}

void PhysicsActor::Tick(sf::Time elapsed)
{
	if (m_Type != Type::STATIC)
	{
		m_DebugXAxisRect.setPosition(m_Pos);
		m_DebugYAxisRect.setPosition(m_Pos);
		for (size_t i = 0; i < m_Fixtures.size(); ++i)
		{
			m_Fixtures[i]->Tick(elapsed);
		}
	}
}

void PhysicsActor::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	for (size_t i = 0; i < m_Fixtures.size(); ++i)
	{
		m_Fixtures[i]->Draw(target, states);
	}
	target.draw(m_DebugXAxisRect, states);	
	target.draw(m_DebugYAxisRect, states);
}

void PhysicsActor::SetPosition(sf::Vector2f newPosition)
{
	m_Pos = newPosition;
}

sf::Vector2f PhysicsActor::GetPosition() const
{
	return m_Pos;
}

void PhysicsActor::SetVelocity(sf::Vector2f newVelocity)
{
	m_Vel = newVelocity;
}

sf::Vector2f PhysicsActor::GetVelocity() const
{
	return m_Vel;
}

void PhysicsActor::AddFixture(Fixture* newFixture)
{
	if (newFixture == nullptr) return;
	m_Fixtures.push_back(newFixture);
}

std::vector<Fixture*> PhysicsActor::GetFixtures() const
{
	return m_Fixtures;
}

void PhysicsActor::SetUserData(int userData)
{
	m_UserData = userData;
}

int PhysicsActor::GetUserData() const
{
	return m_UserData;
}

void PhysicsActor::SetUserPointer(void* userPointer)
{
	m_UserPointer = userPointer;
}

void* PhysicsActor::GetUserPointer() const
{
	return m_UserPointer;
}

void PhysicsActor::SetSolid(bool solid)
{
	m_Solid = solid;
}

bool PhysicsActor::IsSolid() const
{
	return m_Solid;
}

bool PhysicsActor::IsOverlapping(PhysicsActor* otherActor)
{
	const std::vector<Fixture*> otherFixtures = otherActor->GetFixtures();
	for (size_t i = 0; i < m_Fixtures.size(); ++i)
	{
		for (size_t j = 0; j < otherFixtures.size(); ++j)
		{
			if (m_Fixtures[i]->IsOverlapping(otherFixtures[i])) return true;
		}
	}
	return false;
}

PhysicsActor::Type PhysicsActor::GetType() const
{
	return m_Type;
}

float PhysicsActor::GetRestitution() const
{
	return m_Fixtures[0]->GetRestitution();
}
