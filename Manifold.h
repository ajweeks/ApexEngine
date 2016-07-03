#pragma once

#include <SFML\System\Vector2.hpp>

class PhysicsActor;

class Manifold
{
public:
	Manifold(PhysicsActor* A, PhysicsActor* B);
	virtual ~Manifold();

	//Manifold(const Manifold&) = delete;
	//Manifold& operator=(const Manifold&) = delete;

	void Solve();
	void Initialize(float dt);
	void ApplyImpulse();
	void Positionalorrection();
	void InfiniteMassCorrection();

	void SetContactCount(size_t contactCount);
	size_t GetContactCount() const;

	void SetPenetration(float penetration);
	float GetPenetration() const;

	void SetNormal(sf::Vector2f normal);
	sf::Vector2f GetNormal() const;
	
	void SetContact(int contactIndex, sf::Vector2f position);


private:
	PhysicsActor* A;
	PhysicsActor* B;

	float m_Penetration;
	sf::Vector2f m_Normal;
	sf::Vector2f m_Contacts[2];
	size_t m_ContactCount;
	float m_E; // min restitution
	float m_DynamicFriction;
	float m_StaticFriction;

};
