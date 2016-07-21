
#include "Bullet.h"
#include "BulletManager.h"
#include "PhysicsActor.h"
#include "Level.h"
#include "ApexMain.h"

const float Bullet::RADIUS = 2.0f;
const sf::Color Bullet::FILL_COLOR = sf::Color(25, 40, 38);

Bullet::Bullet(Level* level, sf::Vector2f position, float direction, sf::Vector2f additionalVelcity) :
	Projectile(level, position + sf::Vector2f(-4.5f, -4.5f), ActorID::BULLET, this),
	m_Direction(direction)
{
	m_Actor->AddCircleFixture(RADIUS);
	m_Actor->AddContactListener(this);
	m_Actor->SetBullet(true);

	b2Filter collisionFilter;
	collisionFilter.categoryBits = ActorID::BULLET;
	collisionFilter.maskBits = ActorID::WALL | ActorID::SHEEP | ActorID::PLAYER;
	m_Actor->SetCollisionFilter(collisionFilter);

	m_BulletManager = level->GetBulletManager();
	m_BulletManager->AddBullet(this);
	m_Actor->SetLinearVelocity(additionalVelcity + sf::Vector2f(cos(m_Direction) * 60000.0f, sin(m_Direction) * 60000.0f));

	m_Circle.setFillColor(FILL_COLOR);
	m_Circle.setRadius(RADIUS);
}

Bullet::~Bullet()
{
}

void Bullet::Tick(sf::Time elapsed)
{
	sf::Vector2f pos = m_Actor->GetPosition();
	if (pos.x < 0 || pos.x > m_Level->GetWidth() || pos.y < 0 || pos.y > m_Level->GetHeight())
	{
		m_BulletManager->RemoveBullet(this);
		return;
	}
}

void Bullet::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	const sf::Vector2f negRadius(-RADIUS, -RADIUS);
	states.transform.translate(m_Actor->GetPosition() + negRadius);
	target.draw(m_Circle, states);
}

void Bullet::BeginContact(PhysicsActor* thisActor, PhysicsActor* otherActor)
{
	if (!otherActor->IsSensor()) m_BulletManager->AddBulletToBeRemoved(this);
}
