
#include "PhysicsActor.h"
#include "ApexMain.h"

#include <Box2D\Dynamics\b2Body.h>
#include <Box2D\Dynamics\b2world.h>
#include <Box2D\Dynamics\b2world.h>
#include <Box2D\Collision\Shapes\b2PolygonShape.h>
#include <Box2D\Collision\Shapes\b2CircleShape.h>

const int PhysicsActor::SCALE = 100;

PhysicsActor::PhysicsActor(sf::Vector2f pos, b2BodyType bodyType, float angle)
{
	b2BodyDef bodyDef;
	bodyDef.position.Set(pos.x / SCALE, pos.y / SCALE);
	bodyDef.angle = angle;
	bodyDef.type = bodyType;
	m_Body = APEX->GetPhysicsWorld()->CreateBody(&bodyDef);

	m_Body->SetFixedRotation(true);
}

PhysicsActor::~PhysicsActor()
{
	for (b2Fixture* fixturePtr = m_Body->GetFixtureList(); fixturePtr != nullptr; fixturePtr = fixturePtr->GetNext())
	{
		fixturePtr->SetUserData(nullptr);
	}

	APEX->GetPhysicsWorld()->DestroyBody(m_Body);
	m_Body = nullptr;
}

b2Fixture* PhysicsActor::AddBoxFixture(float width, float height, float resitiution, float friction, float density)
{
	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(width / SCALE / 2.0f, height / SCALE / 2.0f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &polygonShape;
	fixtureDef.restitution = resitiution;
	fixtureDef.friction = friction;
	fixtureDef.userData = this;
	fixtureDef.filter = m_CollisionFilter;

	if (m_Body->GetType() == b2_dynamicBody) fixtureDef.density = density;
	else fixtureDef.density = 0.0f;

	b2Fixture* fixture = m_Body->CreateFixture(&fixtureDef);
	if (fixture != nullptr) fixture->SetSensor(m_IsSensor);

	return fixture;
}

b2Fixture* PhysicsActor::AddCircleFixture(float radius, sf::Vector2f offset, float resitiution, float friction, float density)
{
	b2CircleShape circleShape;
	circleShape.m_p.Set(offset.x / SCALE, offset.y/ SCALE);
	circleShape.m_radius = radius / SCALE;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &circleShape;
	fixtureDef.restitution = resitiution;
	fixtureDef.friction = friction;
	fixtureDef.userData = this;
	fixtureDef.filter = m_CollisionFilter;

	if (m_Body->GetType() == b2_dynamicBody) fixtureDef.density = density;
	else fixtureDef.density = 0.0f;

	b2Fixture* fixture = m_Body->CreateFixture(&fixtureDef);
	if (fixture != nullptr) fixture->SetSensor(m_IsSensor);

	return fixture;
}

void PhysicsActor::SetUserData(int data)
{
	m_UserData = data;
}

int PhysicsActor::GetUserData() const
{
	return m_UserData;
}

void PhysicsActor::SetUserPointer(void * ptr)
{
	m_UserPointer = ptr;
}

void* PhysicsActor::GetUserPointer() const
{
	return m_UserPointer;
}

void PhysicsActor::SetPosition(sf::Vector2f pos)
{
	// NOTE: *Don't* scale here, it will be scaled in the overloaded function
	SetPosition(pos.x, pos.y);
}

void PhysicsActor::SetPosition(float x, float y)
{
	m_Body->SetTransform(b2Vec2(x / SCALE, y / SCALE), m_Body->GetAngle());
	m_Body->SetAwake(true);
}

void PhysicsActor::SetXPosition(float xPos)
{
	m_Body->SetTransform(b2Vec2(xPos / SCALE, m_Body->GetPosition().y), m_Body->GetAngle());
}

void PhysicsActor::SetYPosition(float yPos)
{
	m_Body->SetTransform(b2Vec2(m_Body->GetPosition().x, yPos / SCALE), m_Body->GetAngle());
}

sf::Vector2f PhysicsActor::GetPosition() const
{
	const b2Vec2 pos = m_Body->GetPosition();
	return sf::Vector2f(pos.x * SCALE, pos.y * SCALE);
}

void PhysicsActor::SetLinearVelocity(sf::Vector2f vel)
{
	SetLinearVelocity(vel.x / SCALE, vel.y / SCALE);
}

void PhysicsActor::SetLinearVelocity(float xv, float yv)
{
	m_Body->SetLinearVelocity(b2Vec2(xv / SCALE, yv / SCALE));
}

void PhysicsActor::SetXVelocity(float xv)
{
	m_Body->SetLinearVelocity(b2Vec2(xv / SCALE, m_Body->GetLinearVelocity().y));
}

void PhysicsActor::SetYVelocity(float yv)
{
	m_Body->SetLinearVelocity(b2Vec2(m_Body->GetLinearVelocity().x, yv / SCALE));
}

sf::Vector2f PhysicsActor::GetLinearVelocity() const
{
	b2Vec2 vel = m_Body->GetLinearVelocity();
	return sf::Vector2f(vel.x * SCALE, vel.y * SCALE);
}

void PhysicsActor::SetAngle(float angle)
{
	m_Body->SetTransform(m_Body->GetPosition(), angle);
	m_Body->SetAwake(true);
}

float PhysicsActor::GetAngle() const
{
	return m_Body->GetAngle();
}

void PhysicsActor::SetAngularVelocity(float angularVel)
{
	m_Body->SetAngularVelocity(angularVel);
}

float PhysicsActor::GetAngularVelocity() const
{
	return m_Body->GetAngularVelocity();
}

void PhysicsActor::SetFixedRotation(bool fixedRotation)
{
	m_Body->SetFixedRotation(fixedRotation);
}

bool PhysicsActor::IsFixedRotation() const
{
	return m_Body->IsFixedRotation();
}

void PhysicsActor::SetBullet(bool bullet)
{
	m_Body->SetBullet(bullet);
}

bool PhysicsActor::IsBullet() const
{
	return m_Body->IsBullet();
}

void PhysicsActor::SetActive(bool active)
{
	m_Body->SetActive(active);
}

bool PhysicsActor::IsActive() const
{
	return m_Body->IsActive();
}

void PhysicsActor::SetBodyType(b2BodyType bodyType)
{
	m_Body->SetType(bodyType);
}

b2BodyType PhysicsActor::GetBodyType() const
{
	return m_Body->GetType();
}

void PhysicsActor::SetAwake(bool awake)
{
	m_Body->SetAwake(awake);
}

bool PhysicsActor::IsAwake() const
{
	return m_Body->IsAwake();
}

void PhysicsActor::SetSensor(bool isSensor)
{
	m_IsSensor = isSensor;
	for (b2Fixture* fixturePtr = m_Body->GetFixtureList(); fixturePtr != nullptr; fixturePtr = fixturePtr->GetNext())
	{
		fixturePtr->SetSensor(isSensor);
	}
}

bool PhysicsActor::IsSensor() const
{
	return m_IsSensor;
}

void PhysicsActor::SetCollisionFilter(const b2Filter& collisionFilter)
{
	m_CollisionFilter = collisionFilter;
	for (b2Fixture* fixturePtr = m_Body->GetFixtureList(); fixturePtr != nullptr; fixturePtr = fixturePtr->GetNext())
	{
		fixturePtr->SetFilterData(collisionFilter);
	}
}

b2Filter PhysicsActor::GetCollisionFilter() const
{
	return m_CollisionFilter;
}

void PhysicsActor::AddContactListener(ContactListener *listenerPtr)
{
	m_Body->SetUserData(listenerPtr);
}

void PhysicsActor::RemoveContactListener()
{
	m_Body->SetUserData(nullptr);
}

ContactListener *PhysicsActor::GetContactListener() const
{
	return reinterpret_cast <ContactListener *>(m_Body->GetUserData());
}


void PhysicsActor::ApplyForce(sf::Vector2f force, sf::Vector2f offsetPoint)
{
	force /= float(SCALE * SCALE);
	if (offsetPoint.x == 0.0f && offsetPoint.y == 0.0f)
	{
		m_Body->ApplyForceToCenter(b2Vec2(force.x, force.y), true);
	}
	else
	{
		b2Vec2 p = m_Body->GetWorldPoint(b2Vec2(offsetPoint.x / SCALE, offsetPoint.y / SCALE));
		m_Body->ApplyForce(b2Vec2(force.x, force.y), p, true);
	}
}

void PhysicsActor::ApplyTorque(float torque)
{
	torque /= float(SCALE * SCALE);
	m_Body->ApplyTorque(torque, true);
}

void PhysicsActor::ApplyLinearImpulse(sf::Vector2f impulse, sf::Vector2f offsetPoint)
{
	impulse /= float(SCALE * SCALE);
	b2Vec2 p = m_Body->GetWorldPoint(b2Vec2(offsetPoint.x / SCALE, offsetPoint.y / SCALE));
	m_Body->ApplyLinearImpulse(b2Vec2(impulse.x, impulse.y), p, true);
}

void PhysicsActor::ApplyAngularImpulse(float impulse)
{
	impulse /= float(SCALE * SCALE);
	m_Body->ApplyAngularImpulse(impulse, true);
}

b2Body* PhysicsActor::GetBody() const
{
	return m_Body;
}
