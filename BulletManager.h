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

private:
	std::vector<Bullet*> m_Bullets;

};
