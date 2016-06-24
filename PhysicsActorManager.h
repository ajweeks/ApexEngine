#pragma once

#include "SFML\Graphics.hpp"

class PhysicsActor;

class PhysicsActorManager
{
public:
	PhysicsActorManager();
	virtual ~PhysicsActorManager();

	PhysicsActorManager(const PhysicsActorManager&) = delete;
	PhysicsActorManager& operator=(const PhysicsActorManager&) = delete;

	void AddPhysicsActor(PhysicsActor* newActor);
	bool RemovePhysicsActor(PhysicsActor* actor);

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

private:
	std::vector<PhysicsActor*> m_PhysicsActors;

};
