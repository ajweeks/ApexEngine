
#include "Game\World\Bullet.h"
#include "Game\World\BulletManager.h"
#include "Game\World\World.h"
#include "Game\World\Map.h"
#include "Game\Game.h"

#include "Apex\Physics\PhysicsActor.h"

const float Bullet::RADIUS = 2.0f;
const sf::Color Bullet::FILL_COLOR = sf::Color(25, 40, 38);

Bullet::Bullet(World& world, Map& map, sf::Vector2f position, float direction, sf::Vector2f additionalVelcity) :
	Projectile(world, map, position + sf::Vector2f(-4.5f, -4.5f), ActorID::BULLET, this),
	m_Direction(direction),
	m_BulletManager(m_World.GetBulletManager())
{
	m_Actor->AddCircleFixture(RADIUS);
	m_Actor->AddContactListener(this);
	m_Actor->SetBullet(true);
	m_Actor->SetLinearVelocity(additionalVelcity + sf::Vector2f(cos(m_Direction) * 60000.0f, sin(m_Direction) * 60000.0f));

	b2Filter collisionFilter;
	collisionFilter.categoryBits = int(ActorID::BULLET);
	collisionFilter.maskBits = int(ActorID::BUILDING) | int(ActorID::SHEEP) | int(ActorID::PLAYER);
	m_Actor->SetCollisionFilter(collisionFilter);

	m_BulletManager.AddBullet(this);

	m_Circle.setFillColor(FILL_COLOR);
	m_Circle.setRadius(RADIUS);
}

Bullet::~Bullet()
{
}

void Bullet::Tick(sf::Time elapsed)
{
	sf::Vector2f pos = m_Actor->GetPosition();
	Map* currentMap = m_World.GetCurrentMap();
	const size_t tileSize = currentMap->GetTileSize();
	if (pos.x < 0 || pos.x > currentMap->GetTilesWide() * tileSize || pos.y < 0 || pos.y > currentMap->GetTilesHigh() * tileSize)
	{
		m_BulletManager.RemoveBullet(this);
		return;
	}
}

void Bullet::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	const sf::Vector2f negRadius(-RADIUS, -RADIUS);
	states.transform.translate(m_Actor->GetPosition() + negRadius);
	target.draw(m_Circle, states);
}

sf::Vector2f Bullet::GetBottomMiddlePoint()
{
	return m_Actor->GetPosition() + sf::Vector2f(0, RADIUS);
}

void Bullet::BeginContact(apex::Contact* contact)
{
	if (!contact->fixtureB->IsSensor()) m_BulletManager.AddBulletToBeRemoved(this);
}
