
#include "PhysicsBody.h"
#include "Fixture.h"
#include "ApexKeyboard.h"

PhysicsBody::PhysicsBody(sf::Vector2f position) :
	m_Pos(position)
{
}

PhysicsBody::~PhysicsBody()
{
	for (size_t i = 0; i < m_Fixtures.size(); ++i)
	{
		delete m_Fixtures[i];
	}
}

void PhysicsBody::Tick(sf::Time elapsed)
{
	const float dt = elapsed.asSeconds();
	const float dVel = 6500.0f * dt;
	if (ApexKeyboard::IsKeyDown(sf::Keyboard::D) || ApexKeyboard::IsKeyDown(sf::Keyboard::Right))
		m_Vel.x += dVel;
	if (ApexKeyboard::IsKeyDown(sf::Keyboard::A) || ApexKeyboard::IsKeyDown(sf::Keyboard::Left))
		m_Vel.x -= dVel;
	if (ApexKeyboard::IsKeyDown(sf::Keyboard::W) || ApexKeyboard::IsKeyDown(sf::Keyboard::Up))
		m_Vel.y -= dVel;
	if (ApexKeyboard::IsKeyDown(sf::Keyboard::S) || ApexKeyboard::IsKeyDown(sf::Keyboard::Down))
		m_Vel.y += dVel;

	if (m_Vel.x != 0 || m_Vel.y != 0)
	{
		m_Vel.x *= 1.0f - (5.0f * dt);
		m_Vel.y *= 1.0f - (5.0f * dt);

		static const float epsilon = 0.001f;
		if (abs(m_Vel.x) < epsilon) m_Vel.x = 0.0f;
		if (abs(m_Vel.y) < epsilon) m_Vel.y = 0.0f;

		m_Pos += m_Vel * elapsed.asSeconds();
	}
}

void PhysicsBody::Draw(sf::RenderTarget& target, sf::RenderStates states)
{

}

void PhysicsBody::SetPosition(sf::Vector2f newPosition)
{
	m_Pos = newPosition;
}

sf::Vector2f PhysicsBody::GetPosition() const
{
	return m_Pos;
}

void PhysicsBody::SetVelocity(sf::Vector2f newVelocity)
{
	m_Vel = newVelocity;
}

sf::Vector2f PhysicsBody::GetVelocity() const
{
	return m_Vel;
}

void PhysicsBody::AddFixture(Fixture* newFixture)
{
	if (newFixture == nullptr) return;
	m_Fixtures.push_back(newFixture);
}

std::vector<Fixture*> PhysicsBody::GetFixtures() const
{
	return m_Fixtures;
}

void PhysicsBody::SetUserData(int userData)
{
	m_UserData = userData;
}

int PhysicsBody::GetUserData() const
{
	return m_UserData;
}

void PhysicsBody::SetUserPointer(void* userPointer)
{
	m_UserPointer = userPointer;
}

void * PhysicsBody::GetUserPointer() const
{
	return m_UserPointer;
}

