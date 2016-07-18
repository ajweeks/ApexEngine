#pragma once

#include <SFML\Graphics.hpp>

class Bullet;

class BulletManager
{
public:
	BulletManager();
	virtual ~BulletManager();

	BulletManager(const BulletManager&) = delete;
	BulletManager& operator=(const BulletManager&) = delete;

	void Reset();

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

	void AddBullet(Bullet* bullet);
	void RemoveBullet(Bullet* bullet);
	void AddBulletToBeRemoved(Bullet* bullet); // Call this when Box2D is locked, these bullets will be deleted next tick

private:
	std::vector<Bullet*> m_Bullets;
	std::vector<Bullet*> m_BulletsToBeRemoved;

};
