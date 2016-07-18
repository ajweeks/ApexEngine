#pragma once

#include <Box2D\Dynamics\b2World.h>
#include <Box2D\Dynamics\b2Body.h>

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

class ApexDebugDraw;

class PhysicsActorManager
{
public:
	PhysicsActorManager(sf::RenderTarget& target);
	virtual ~PhysicsActorManager();

	PhysicsActorManager(const PhysicsActorManager&) = delete;
	PhysicsActorManager& operator=(const PhysicsActorManager&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target);

	b2World* GetWorld() const;

	static const float TIMESTEP;
private:
	static const int POS_ITERATIONS;
	static const int VEL_ITERATIONS;

	b2World* m_World = nullptr;
	ApexDebugDraw* m_DebugDraw = nullptr;

};