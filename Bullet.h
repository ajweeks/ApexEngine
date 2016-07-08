#pragma once

#include "Projectile.h"
#include <SFML\Graphics\CircleShape.hpp>
#include <SFML\Graphics\RenderStates.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

class BulletManager;

class Bullet : public Projectile
{
public:
	Bullet(Level* level, sf::Vector2f position, float direction, sf::Vector2f additionalVelcity = sf::Vector2f());
	virtual ~Bullet();

	Bullet(const Bullet&) = delete;
	Bullet& operator=(const Bullet&) = delete;

	virtual void Tick(sf::Time elapsed);
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states);

private:
	static const float RADIUS;

	BulletManager* m_BulletManager = nullptr;
	float m_Direction; // Angle in radians
	sf::CircleShape m_Circle;

};
