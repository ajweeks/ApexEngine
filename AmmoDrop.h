#pragma once

#include "Item.h"

class AmmoDrop : public Item
{
public:
	AmmoDrop(World* world, sf::Vector2f position, int bulletCount);
	virtual ~AmmoDrop();

	AmmoDrop(const AmmoDrop&) = delete;
	AmmoDrop& operator=(const AmmoDrop&) = delete;

	int GetBulletCount() const;

	virtual void Tick(sf::Time elapsed) override;
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states) override;

	virtual void BeginContact(PhysicsActor* thisActor, PhysicsActor* otherActor) override;
	virtual void EndContact(PhysicsActor* thisActor, PhysicsActor* otherActor)  override {};
	virtual void PreSolve(PhysicsActor* thisActor, PhysicsActor* otherActor, bool& enableContact) override {};

private:
	static const float WIDTH;
	static const float HEIGHT;

	int m_BulletCount;
	sf::Sprite m_Sprite;
	// Bullet type?
	// Lifetime?

};
