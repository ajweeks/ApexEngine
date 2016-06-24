
#include "Bullet.h"
#include "BulletManager.h"
#include "PhysicsActor.h"
#include "Level.h"

Bullet::Bullet(Level* level, sf::Vector2f position, float direction, sf::Vector2f additionalVelcity) :
	Projectile(level, position + sf::Vector2f(-4.5f, -4.5f), Type::BULLET, this),
	m_Direction(direction)
{
	m_BulletManager = level->GetBulletManager();
	m_BulletManager->AddBullet(this);
	m_Actor->SetVelocity(additionalVelcity + sf::Vector2f(cos(m_Direction) * 600.0f, sin(m_Direction) * 600.0f));

	m_Circle.setFillColor(sf::Color(180, 78, 40));
	m_Circle.setRadius(7.0f);
}

Bullet::~Bullet()
{
}

void Bullet::Tick(sf::Time elapsed)
{
	sf::Vector2f pos = m_Actor->GetPosition();
	if (pos.x < 0 || pos.x > 2080 || pos.y < 0 || pos.y > 1216)
	{
		m_BulletManager->RemoveBullet(this);
		return;
	}

	m_Circle.setPosition(m_Actor->GetPosition());
}

void Bullet::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	target.draw(m_Circle, states);
}
