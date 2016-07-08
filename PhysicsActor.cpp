
#include "PhysicsActor.h"
#include "Level.h"
#include "PhysicsActorManager.h"
#include "ApexMath.h"
#include "Fixture.h"

PhysicsActor::PhysicsActor(sf::Vector2f centerPos, BodyType bodyType, Level* level) :
	m_Level(level), m_BodyType(bodyType)
{
	level->GetActorManager()->AddPhysicsActor(this);
	m_Pos = centerPos;
	m_Solid = true;

	m_DebugXAxisRect = sf::RectangleShape(sf::Vector2f(15, 2));
	m_DebugXAxisRect.setFillColor(sf::Color::Red);
	m_DebugXAxisRect.setPosition(m_Pos);
	m_DebugYAxisRect = sf::RectangleShape(sf::Vector2f(2, 15));
	m_DebugYAxisRect.setFillColor(sf::Color::Green);
	m_DebugYAxisRect.setPosition(m_Pos);
	if (m_BodyType == BodyType::STATIC) 
	{
		m_InverseIntertia = 0.0f;
		m_InverseMass = 0.0f;
	}
	else 
	{
		m_InverseIntertia = 1.0f / 5.0f;
		m_InverseMass = 1.0f / 2.0f;
	}
}

PhysicsActor::~PhysicsActor()
{
	m_Level->GetActorManager()->RemovePhysicsActor(this);
	delete m_Fixture;
}

void PhysicsActor::Tick(sf::Time elapsed)
{
	if (m_BodyType != BodyType::STATIC)
	{
		m_DebugXAxisRect.setPosition(m_Pos);
		m_DebugYAxisRect.setPosition(m_Pos);
	}
	m_Fixture->Tick(elapsed);
}

void PhysicsActor::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	target.draw(m_DebugXAxisRect, states);
	target.draw(m_DebugYAxisRect, states);
	if (m_Fixture != nullptr) m_Fixture->Draw(target, states);
	SetOverlapping(false);
}

void PhysicsActor::ApplyForce(const sf::Vector2f& force)
{
	m_Force += force;
}

void PhysicsActor::ApplyImpulse(const sf::Vector2f& impulse, const sf::Vector2f& contactVector)
{
	m_Vel += m_InverseMass * impulse;
	m_AngularVel += m_InverseIntertia * ApexMath::Cross(contactVector, impulse);
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

void PhysicsActor::SetAngularVelocity(float angularVelocity)
{
	m_AngularVel = angularVelocity;
}

float PhysicsActor::GetAngularVelocity() const
{
	return m_AngularVel;
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

PhysicsActor::BodyType PhysicsActor::GetBodyType() const
{
	return m_BodyType;
}

float PhysicsActor::GetRestitution() const
{
	return m_Restitution;
}

void PhysicsActor::SetOverlapping(bool overlapping)
{
	m_IsOverlapping = overlapping;
}

void PhysicsActor::SetInverseMass(float inverseMass)
{
	m_InverseMass = inverseMass;
}

float PhysicsActor::GetInverseMass() const
{
	return m_InverseMass;
}

void PhysicsActor::SetInverseInertia(float inverseIntertia)
{
	m_InverseIntertia = inverseIntertia;
}

float PhysicsActor::GetInverseInertia() const
{
	return m_InverseIntertia;
}

void PhysicsActor::SetOrientation(float orientation)
{
	m_Orientation = orientation;
}

float PhysicsActor::GetOrientation() const
{
	return m_Orientation;
}

void PhysicsActor::SetFixture(Fixture* fixture)
{
	m_Fixture = fixture;
}

Fixture* PhysicsActor::GetFixture() const
{
	return m_Fixture;
}

float PhysicsActor::GetStaticFriction() const
{
	return m_StaticFriction;
}

float PhysicsActor::GetDynamicFriction() const
{
	return m_DynamicFriction;
}

sf::Vector2f PhysicsActor::GetForce() const
{
	return m_Force;
}

void PhysicsActor::SetForce(sf::Vector2f force)
{
	m_Force = force;
}

float PhysicsActor::GetTorque() const
{
	return m_Torque;
}

void PhysicsActor::SetTorque(float torque)
{
	m_Torque = torque;
}
