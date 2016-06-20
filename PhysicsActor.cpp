
#include "PhysicsActor.h"
#include "PhysicsBody.h"

PhysicsActor::PhysicsActor(sf::Vector2f position)
{
	m_Body = new PhysicsBody(position);
}

PhysicsActor::~PhysicsActor()
{
	delete m_Body;
}

void PhysicsActor::Tick(sf::Time elapsed)
{
	// Solve collisions in collection class?
	m_Body->Tick(elapsed);
}

void PhysicsActor::draw(sf::RenderTarget& target, sf::RenderStates states)
{
	m_Body->Draw(target, states);

	sf::RectangleShape xAxis(sf::Vector2f(4, 60));
	xAxis.setFillColor(sf::Color::Red);
	target.draw(xAxis, states);

	sf::RectangleShape yAxis(sf::Vector2f(60, 4));
	yAxis.setFillColor(sf::Color::Green);
	target.draw(yAxis, states);
}

void PhysicsActor::SetPosition(sf::Vector2f newPosition)
{
	m_Body->SetPosition(newPosition);
}

sf::Vector2f PhysicsActor::GetPosition() const
{
	return m_Body->GetPosition();
}

void PhysicsActor::GetVelocity(sf::Vector2f newVelocity)
{
	m_Body->SetVelocity(newVelocity);
}

sf::Vector2f PhysicsActor::GetVelocity() const
{
	return m_Body->GetVelocity();
}
