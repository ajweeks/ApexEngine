#pragma once

#include "Projectile.h"

#include <SFML\Graphics\CircleShape.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

class BulletManager;

class Bullet : public Projectile
{
public:
	Bullet(World& world, Map& map, sf::Vector2f position, float direction, sf::Vector2f additionalVelcity = sf::Vector2f());
	virtual ~Bullet();

	Bullet(const Bullet&) = delete;
	Bullet& operator=(const Bullet&) = delete;

	virtual void Tick(sf::Time elapsed) override;
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states) override;

	virtual sf::Vector2f GetBottomMiddlePoint() override;
	virtual void BeginContact(apex::Contact* contact) override;

private:
	static const float RADIUS;
	static const sf::Color FILL_COLOR;

	BulletManager& m_BulletManager;

	float m_Direction; // Angle in radians
	sf::CircleShape m_Circle;
};
