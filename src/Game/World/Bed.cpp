
#include "Game\World\Bed.h"
#include "Game\World\Layer.h"
#include "Game\World\Map.h"

#include "Apex\Graphics\TextureManager.h"
#include "Apex\Physics\PhysicsActor.h"

Bed::Bed(World& world, Map& map, sf::Vector2f position) :
	Item(world, map, position + sf::Vector2f(TILES_WIDE * map.GetTileSize() / 2.0f, TILES_HIGH * map.GetTileSize() / 2.0f),
		ActorID::BED, this, b2BodyType::b2_staticBody),
	m_Texture(apex::TextureManager::GetTexture(Texture::GENERAL_TILES)),
	m_Verticies(Map::GetSpriteTexture(TILES_WIDE, TILES_HIGH, map.GetTileSet(), TILE_X_OFFSET, TILE_Y_OFFSET, position))
{
}

Bed::~Bed()
{
}

void Bed::Tick(sf::Time elapsed)
{
}

void Bed::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	states.texture = m_Texture;
	target.draw(m_Verticies, states);
}

sf::Vector2f Bed::GetBottomMiddlePoint()
{
	return m_Actor->GetPosition() + sf::Vector2f(0, TILES_HIGH * m_Map.GetTileSize() / 2.0f);
}

void Bed::CreatePhysicsActor()
{
	Item::CreatePhysicsActor();
	b2Filter collisionFilter;
	collisionFilter.categoryBits = int(ActorID::BED);
	collisionFilter.maskBits = int(ActorID::PLAYER) | int(ActorID::AI_CHARACTER) | int(ActorID::COIN);
	m_Actor->SetCollisionFilter(collisionFilter);
	// TODO: Use global tilesize
	m_Actor->AddBoxFixture(float(TILES_WIDE * m_Map.GetTileSize()), float((TILES_HIGH - 1.0f) * m_Map.GetTileSize()));
}
