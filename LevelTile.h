#pragma once

#include "Entity.h"

#include <Box2D\Dynamics\b2Body.h>

class LevelTile : public Entity
{
public:
	LevelTile(Level* level, sf::Vector2f position, ActorID id);
	virtual ~LevelTile();

	LevelTile(const LevelTile&) = delete;
	LevelTile& operator=(const LevelTile&) = delete;

	virtual void Tick(sf::Time elapsed);
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states);

	virtual void BeginContact(PhysicsActor* thisActor, PhysicsActor* otherActor) override;
	virtual void EndContact(PhysicsActor* thisActor, PhysicsActor* otherActor)  override;
	virtual void PreSolve(PhysicsActor* thisActor, PhysicsActor* otherActor, bool& enableContact) override;

private:

};
