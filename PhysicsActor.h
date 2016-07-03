#pragma once

#include <SFML\System\Vector2.hpp>
#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RectangleShape.hpp>

class Level;
class Fixture;

class PhysicsActor
{
public:
	enum class BodyType
	{
		STATIC, DYNAMIC //, KINEMATIC?
	};

	PhysicsActor(sf::Vector2f centerPos, BodyType bodyType, Level* level);
	virtual ~PhysicsActor();

	PhysicsActor(const PhysicsActor&) = delete;
	PhysicsActor& operator=(const PhysicsActor&) = delete;
	
	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

	void ApplyForce(const sf::Vector2f& force);
	void ApplyImpulse(const sf::Vector2f& impulse, const sf::Vector2f& contactVector);

	void SetPosition(sf::Vector2f newPosition);
	sf::Vector2f GetPosition() const;

	void SetVelocity(sf::Vector2f newVelocity);
	sf::Vector2f GetVelocity() const;
	
	void SetAngularVelocity(float angularVelocity);
	float GetAngularVelocity() const;

	void SetUserData(int userData);
	int GetUserData() const;

	void SetUserPointer(void* userPointer);
	void* GetUserPointer() const;

	void SetSolid(bool solid);
	bool IsSolid() const;
	
	BodyType GetBodyType() const;

	float GetRestitution() const;
	void SetOverlapping(bool overlapping);

	void SetInverseMass(float inverseMass);
	float GetInverseMass() const;

	void SetInverseInertia(float inverseIntertia);
	float GetInverseInertia() const;
	
	void SetOrientation(float orientation);
	float GetOrientation() const;

	void SetFixture(Fixture* fixture);
	Fixture* GetFixture() const;

	float GetStaticFriction() const;
	float GetDynamicFriction() const;

	sf::Vector2f GetForce() const;
	void SetForce(sf::Vector2f force);

	float GetTorque() const;
	void SetTorque(float torque);

private:
	sf::Vector2f m_Pos;
	sf::Vector2f m_Vel;
	float m_AngularVel;
	float m_Torque;
	float m_Orientation; // in radians
	sf::Vector2f m_Force;
	float m_InverseMass;
	float m_InverseIntertia;

	float m_StaticFriction;
	float m_DynamicFriction;
	float m_Restitution;

	BodyType m_BodyType;
	bool m_Solid;

	sf::RectangleShape m_DebugXAxisRect;
	sf::RectangleShape m_DebugYAxisRect;

	Level* m_Level = nullptr;
	Fixture* m_Fixture = nullptr;

	int m_UserData;
	void* m_UserPointer = nullptr;

	bool m_IsOverlapping = false;

};

