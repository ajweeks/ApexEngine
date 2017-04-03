#pragma once

#include "Item.h"

class AmmoDrop : public Item
{
public:
	AmmoDrop(World& world, Map& map, sf::Vector2f position, int bulletCount);
	virtual ~AmmoDrop();

	AmmoDrop(const AmmoDrop&) = delete;
	AmmoDrop& operator=(const AmmoDrop&) = delete;

	int GetBulletCount() const;

	virtual void Tick(sf::Time elapsed) override;
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states) override;

	virtual void BeginContact(apex::Contact* contact) override;
	virtual void EndContact(apex::Contact* contact)  override {};
	virtual void PreSolve(apex::Contact* contact, bool& enableContact) override {};

private:
	static const float WIDTH;
	static const float HEIGHT;

	int m_BulletCount;
	sf::Sprite m_Sprite;
	// Bullet type?
	// Lifetime?

};
