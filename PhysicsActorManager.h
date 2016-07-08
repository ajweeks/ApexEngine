#pragma once

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\RenderStates.hpp>
#include <vector>

class PhysicsActor;
class Manifold;

class PhysicsActorManager
{
public:
	PhysicsActorManager();
	virtual ~PhysicsActorManager();

	PhysicsActorManager(const PhysicsActorManager&) = delete;
	PhysicsActorManager& operator=(const PhysicsActorManager&) = delete;

	void AddPhysicsActor(PhysicsActor* actor);
	bool RemovePhysicsActor(PhysicsActor* actor);

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

private:
	void IntegrateForces(PhysicsActor* actor, sf::Time elapsed);
	void IntegrateVelocity(PhysicsActor* actor, sf::Time elapsed);

	std::vector<PhysicsActor*> m_PhysicsActors;
	std::vector<Manifold> m_Contacts;

	int m_Iterations = 2;

};
