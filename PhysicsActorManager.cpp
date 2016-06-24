
#include "PhysicsActorManager.h"
#include "PhysicsActor.h"
#include "Player.h"

PhysicsActorManager::PhysicsActorManager()
{
}

PhysicsActorManager::~PhysicsActorManager()
{
}

void PhysicsActorManager::AddPhysicsActor(PhysicsActor* newActor)
{
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
	for (size_t i = 0; i < m_PhysicsActors.size(); ++i)
	{
		if (m_PhysicsActors[i] != nullptr)
		{
			if (m_PhysicsActors[i]->GetType() != PhysicsActor::Type::STATIC)
			{
				m_PhysicsActors[i]->Tick(elapsed);
				m_PhysicsActors[i]->SetPosition(m_PhysicsActors[i]->GetPosition() + (m_PhysicsActors[i]->GetVelocity() * elapsed.asSeconds()));
			}
			continue;

			for (size_t j = 0; j < m_PhysicsActors.size(); ++j)
			{
				if (m_PhysicsActors[j] != nullptr && 
					m_PhysicsActors[j]->GetType() != PhysicsActor::Type::STATIC && 
					m_PhysicsActors[j] != m_PhysicsActors[i])
				{
					if (m_PhysicsActors[i]->IsOverlapping(m_PhysicsActors[j]))
					{
						/*//Calculate relative velocity
						Vec2 rv = B.velocity - A.velocity
 
						// Calculate relative velocity in terms of the normal direction
						float velAlongNormal = DotProduct( rv, normal )
 
						// Do not resolve if velocities are separating
						if(velAlongNormal > 0)
						return;
 
						// Calculate restitution
						float e = min( A.restitution, B.restitution)
 
						// Calculate impulse scalar
						float j = -(1 + e) * velAlongNormal
						j /= 1 / A.mass + 1 / B.mass
 
						// Apply impulse
						Vec2 impulse = j * normal
						A.velocity -= 1 / A.mass * impulse
						B.velocity += 1 / B.mass * impulse*/
						/*PhysicsActor* A = m_PhysicsActors[i];
						PhysicsActor* B = m_PhysicsActors[j];
						const sf::Vector2f dv = A->GetVelocity() - B->GetVelocity();
						float velocityAlongNormal = dv / std::sqrt(dv.x * dv.x + dv.y * dv.y);
						float e = std::min(A->GetRestitution(), B->GetRestitution());
						float j = -(1 + e) * vel;*/
					}
				}
			}
		}
	}

}

void PhysicsActorManager::Draw(sf::RenderTarget & target, sf::RenderStates states)
{
	for (size_t i = 0; i < m_PhysicsActors.size(); ++i)
	{
		if (m_PhysicsActors[i] != nullptr)
		{
			m_PhysicsActors[i]->Draw(target, states);
		}
	}
}
