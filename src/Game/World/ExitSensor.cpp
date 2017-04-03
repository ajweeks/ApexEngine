
#include "Game\World\ExitSensor.h"
#include "Game\World\Map.h"

#include "Apex\Physics\PhysicsActor.h"
#include "Apex\Graphics\TextureManager.h"

ExitSensor::ExitSensor(World& world, Map& map, sf::Vector2f position) :
	Entity(world, map, position + sf::Vector2f(TILES_WIDE * map.GetTileSize() / 2.0f, (TILES_HIGH + 2.0f) * map.GetTileSize() / 2.0f),
		ActorID::EXIT, this, b2BodyType::b2_staticBody)
{
	m_Texture = apex::TextureManager::GetTexture(Texture::GENERAL_TILES);
	m_Verticies = Map::GetSpriteTexture(TILES_WIDE, TILES_HIGH, map.GetTileSet(), sf::Vector2u(TILE_X_OFFSET, TILE_Y_OFFSET), position);
}

ExitSensor::~ExitSensor()
{
}

void ExitSensor::Tick(sf::Time elapsed)
{
}

void ExitSensor::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	states.texture = m_Texture;
	target.draw(m_Verticies, states);
}

sf::Vector2f ExitSensor::GetBottomMiddlePoint()
{
	// Draw on top of everything else
	return sf::Vector2f(0, 0);
}

void ExitSensor::CreatePhysicsActor()
{
	Entity::CreatePhysicsActor();
	const int tileSize = m_Map.GetTileSize();
	m_Actor->AddBoxFixture(float(TILES_WIDE * tileSize), float(TILES_HIGH * tileSize), true);

	b2Filter collisionFilter;
	collisionFilter.categoryBits = int(ActorID::EXIT);
	collisionFilter.maskBits |= int(ActorID::PLAYER) | int(ActorID::AI_CHARACTER);
	m_Actor->SetCollisionFilter(collisionFilter);
}
