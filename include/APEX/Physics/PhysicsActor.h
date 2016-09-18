#pragma once

#include <Box2D\Dynamics\b2Fixture.h>

#include <SFML\System\Vector2.hpp>

class b2Body;

namespace apex
{
	class ContactListener;

	class PhysicsActor
	{
	public:
		PhysicsActor(sf::Vector2f pos, b2BodyType bodyType, float angle = 0.0f);
		virtual ~PhysicsActor();

		PhysicsActor(const PhysicsActor&) = delete;
		PhysicsActor& operator=(const PhysicsActor&) = delete;

		b2Fixture* AddBoxFixture(float width, float height, bool isSensor = false, const b2Vec2& center = b2Vec2(0.0f, 0.0f), float resitiution = 0.8f, float friction = 0.7f, float density = 1.0f);
		b2Fixture* AddCircleFixture(float radius, bool isSensor = false, const b2Vec2& offset = b2Vec2(0.0f, 0.0f), float resitiution = 0.8f,
			float friction = 0.7f, float density = 1.0f);

		void SetUserData(int userData);
		int GetUserData() const;

		void SetUserPointer(void* userPointer);
		void* GetUserPointer() const;

		b2Body* GetBody() const;

		void SetPosition(sf::Vector2f pos);
		void SetPosition(float x, float y);
		void SetXPosition(float xPos);
		void SetYPosition(float yPos);
		sf::Vector2f GetPosition() const;

		void SetLinearVelocity(sf::Vector2f vel);
		void SetLinearVelocity(float xv, float yv);
		void SetXVelocity(float xv);
		void SetYVelocity(float yv);
		sf::Vector2f GetLinearVelocity() const;

		void SetAngle(float angle);
		float GetAngle() const;

		void SetAngularVelocity(float angularVel);
		float GetAngularVelocity() const;

		void SetFixedRotation(bool hasFixedRotation);
		bool IsFixedRotation() const;

		void SetBullet(bool isBullet);
		bool IsBullet() const;

		void SetBodyType(b2BodyType bodyType);
		b2BodyType GetBodyType() const;

		void SetAwake(bool awake);
		bool IsAwake() const;

		void SetActive(bool active);
		bool IsActive() const;

		void SetCollisionFilter(const b2Filter& collisionFilter);
		b2Filter GetCollisionFilter() const;

		void AddContactListener(ContactListener* listener);
		void RemoveContactListener();
		ContactListener* GetContactListener() const;

		void ApplyForce(sf::Vector2f force, sf::Vector2f offsetPoint);
		void ApplyTorque(float torque);
		void ApplyLinearImpulse(sf::Vector2f impulse, sf::Vector2f offsetPoint);
		void ApplyAngularImpulse(float impulse);

		b2Fixture* GetFixtureList() const;

		static const float SCALE;
	private:
		int m_UserData = 0;
		void* m_UserPointer = nullptr;

		b2Body* m_Body = nullptr;
		b2Filter m_CollisionFilter;
	};
} // namespace apex
