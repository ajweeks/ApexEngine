#pragma once

#include "Entity.h"

class SolidBlock : public Entity
{
public:
	SolidBlock(World& world, Map& map, sf::Vector2f position);
	virtual ~SolidBlock();

	SolidBlock(const SolidBlock&) = delete;
	SolidBlock& operator=(const SolidBlock&) = delete;

	void Tick(sf::Time elapsed) override;
	void Draw(sf::RenderTarget& target, sf::RenderStates states) override;

	virtual void CreatePhysicsActor() override;
	virtual sf::Vector2f GetBottomMiddlePoint() override;

private:

};
