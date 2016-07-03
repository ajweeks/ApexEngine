
#include "PhysicsActorManager.h"
#include "PhysicsActor.h"
#include "Player.h"
#include "Manifold.h"
#include <iostream>

PhysicsActorManager::PhysicsActorManager()
{
}

PhysicsActorManager::~PhysicsActorManager()
{
}

void PhysicsActorManager::AddPhysicsActor(PhysicsActor* newActor)
{
	for (size_t i = 0; i < m_PhysicsActors.size(); ++i)
	{
		if (m_PhysicsActors[i] == nullptr)
		{
			m_PhysicsActors[i] = newActor;
			return;
		}
	}
	m_PhysicsActors.push_back(newActor);
}

bool PhysicsActorManager::RemovePhysicsActor(PhysicsActor* actor)
{
	for (size_t i = 0; i < m_PhysicsActors.size(); ++i)
	{
		if (m_PhysicsActors[i] == actor)
		{
			m_PhysicsActors[i] = nullptr;
			return true;
		}
	}
	return false;
}

void PhysicsActorManager::Tick(sf::Time elapsed)
{
	const size_t numActors = m_PhysicsActors.size();
	const float dt = elapsed.asSeconds();
	m_Contacts.clear();
	for (size_t i = 0; i < numActors; ++i)
	{
		PhysicsActor* A = m_PhysicsActors[i];
		if (A != nullptr)
		{
			if (A->GetBodyType() != PhysicsActor::BodyType::STATIC)
			{
				A->Tick(elapsed);
				A->SetPosition(A->GetPosition() + (A->GetVelocity() * dt));

				if (A->IsSolid())
				{
					for (size_t j = 0; j < numActors; ++j)
					{
						PhysicsActor* B = m_PhysicsActors[j];
						if (B != nullptr && B->IsSolid())
						{
							if (A->GetInverseMass() == 0.0f && B->GetInverseMass() == 0.0f)
								continue;

							Manifold m(A, B);
							m.Solve();

							if (m.GetContactCount())
							{
								// Why not push_back?
								m_Contacts.emplace_back(m);
							}
						}
					}
				}
			}
		}
	}

	for (size_t i = 0; i < numActors; ++i)
	{
		if (m_PhysicsActors[i] != nullptr) IntegrateForces(m_PhysicsActors[i], elapsed);
	}

	for (size_t i = 0; i < m_Contacts.size(); ++i)
		m_Contacts[i].Initialize(dt);

	for (int j = 0; j < m_Iterations; ++j)
		for (size_t i = 0; i < m_Contacts.size(); ++i)
			m_Contacts[i].ApplyImpulse();

	for (size_t i = 0; i < numActors; ++i)
	{
		if (m_PhysicsActors[i] != nullptr) IntegrateVelocity(m_PhysicsActors[i], elapsed);
	}

	for (size_t i = 0; i < m_Contacts.size(); ++i)
		m_Contacts[i].Positionalorrection();

	for (size_t i = 0; i < numActors; ++i)
	{
		if (m_PhysicsActors[i] != nullptr)
		{
			m_PhysicsActors[i]->SetForce(sf::Vector2f(0.0f, 0.0f));
			m_PhysicsActors[i]->SetTorque(0.0f);
		}
	}
}

/*

void PhysicsActorManager::Tick(sf::Time elapsed)
{
const size_t numActors = m_PhysicsActors.size();
const float dt = elapsed.asSeconds();
for (size_t i = 0; i < numActors; ++i)
{
if (m_PhysicsActors[i] != nullptr)
{
PhysicsActor* A = m_PhysicsActors[i];
if (A->GetBodyType() != PhysicsActor::BodyType::STATIC)
{
A->Tick(elapsed);
A->SetPosition(A->GetPosition() + (A->GetVelocity() * dt));

for (size_t j = 0; j < numActors; ++j)
{
if (j == i) continue;

if (m_PhysicsActors[j] != nullptr)
{
PhysicsActor* B = m_PhysicsActors[j];
if (A->IsOverlapping(B))
{
A->SetOverlapping(true);
B->SetOverlapping(true);

const sf::Vector2f deltaVel = A->GetVelocity() - B->GetVelocity();
const sf::Vector2f normal = deltaVel;
float velocityAlongNormal = deltaVel.x + deltaVel.y;

if (velocityAlongNormal < 0) return; // The objects are already moving away from each other

float e = std::min(A->GetRestitution(), B->GetRestitution());
float j = -(1 + e) * velocityAlongNormal;
j /= A->GetInverseMass() + B->GetInverseMass();

sf::Vector2f impluse = j * normal;
A->SetVelocity(A->GetVelocity() - A->GetInverseMass() * impluse);
B->SetVelocity(B->GetVelocity() + B->GetInverseMass() * impluse);

const float percent = 0.2f;
const float slop = 0.01f;
sf::Vector2f correction = std::max(1.0f - slop, 0.0f) / (A->GetInverseMass() + B->GetInverseMass()) * percent * normal;
A->SetPosition(A->GetPosition() - A->GetInverseMass() * correction);
B->SetPosition(B->GetPosition() + B->GetInverseMass() * correction);
}
}
}
}
}
}

}

*/

void PhysicsActorManager::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	for (size_t i = 0; i < m_PhysicsActors.size(); ++i)
	{
		if (m_PhysicsActors[i] != nullptr)
		{
			m_PhysicsActors[i]->Draw(target, states);
		}
	}
}

void PhysicsActorManager::IntegrateForces(PhysicsActor* actor, sf::Time elapsed)
{
	if (actor->GetInverseMass() == 0.0f)
		return;

	const float dt = elapsed.asSeconds();
	actor->SetVelocity(actor->GetVelocity() + (actor->GetForce() * actor->GetInverseMass()));// +  friction?);
	actor->SetAngularVelocity(actor->GetAngularVelocity() + actor->GetTorque() * actor->GetInverseInertia() * (dt / 2.0f));
}

void PhysicsActorManager::IntegrateVelocity(PhysicsActor* actor, sf::Time elapsed)
{
	if (actor->GetInverseMass() == 0.0f)
		return;

	const float dt = elapsed.asSeconds();
	actor->SetPosition(actor->GetPosition() + actor->GetVelocity() * dt);
	actor->SetOrientation(actor->GetAngularVelocity() * dt);
	IntegrateForces(actor, elapsed);
}
