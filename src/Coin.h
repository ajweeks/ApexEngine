#pragma once

#include "Item.h"
#include "ApexSpriteSheet.h"

class Coin : public Item
{
public:
	Coin(World* world, sf::Vector2f position);
	virtual ~Coin();

	Coin& operator=(const Coin&) = delete;
	Coin(const Coin&) = delete;

	void Tick(sf::Time elapsed) override;
	void Draw(sf::RenderTarget& target, sf::RenderStates states) override;

	void BeginContact(PhysicsActor* thisActor, PhysicsActor* otherActor) override;

private:
	ApexSpriteSheet m_SpriteSheet;

};