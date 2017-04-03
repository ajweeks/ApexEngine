
#include "Game\World\Building.h"
#include "Game\World\Map.h"
#include "Game\Game.h"

#include "Apex\ApexMain.h"
#include "Apex\ApexMath.h"
#include "Apex\Graphics\TextureManager.h"

Building::Building(World& world, Map& map, sf::Vector2f position, sf::Texture* texture, sf::Vector2u size, sf::Vector2u tileXY, sf::Vector2f doorPos)
	: Entity(world, map, position, ActorID::BUILDING, this, b2BodyType::b2_staticBody),
	m_Texture(texture),
	m_TilesWide(size.x),
	m_TilesHigh(size.y)
{
	m_Index = 0; // TOOD: Determine at runtime

	const float tileSize = float(m_Map.GetTileSize());
	m_Position += sf::Vector2f(m_TilesWide * tileSize / 2.0f, m_TilesHigh * tileSize * 0.75f);
	m_DoorPosition = doorPos * tileSize;
	m_DoorPosition -= sf::Vector2f(((m_TilesWide - 2) / 2.0f - 0.5f) * tileSize, -0.75f * tileSize - 0.25f * tileSize);

	m_Verticies = Map::GetSpriteTexture(m_TilesWide, m_TilesHigh, map.GetTileSet(), tileXY, position);

	m_DoorHintText = sf::Text("E", Game::FontPixelFJ8, 30);
	m_DoorHintText.setFillColor(sf::Color(240, 240, 244));
	m_DoorHintText.setScale(0.2f, 0.2f);
}

Building::~Building()
{
}

void Building::Tick(sf::Time elapsed)
{
	Player* player = m_World.GetPlayer();
	if (player->GetDoorIndexTouching() != -1)
	{
		if (m_DoorLocked)
		{
			m_DoorHintText.setString("Locked!");
		}
		else
		{
			m_DoorHintText.setString("E");
		}
		m_DoorHintText.setPosition(player->GetPosition() + sf::Vector2f(0, -40.0f));
		m_DoorHintText.setFillColor(sf::Color(240, 240, 244));
	}
	else
	{
		m_DoorHintText.setFillColor(sf::Color::Transparent);
	}
}

void Building::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	states.texture = m_Texture;
	target.draw(m_Verticies, states);

	states.texture = states.Default.texture;
	target.draw(m_DoorHintText, states);
}

void Building::CreatePhysicsActor()
{
	const float tileSize = float(m_Map.GetTileSize());
	Entity::CreatePhysicsActor();
	m_MainFixture = m_Actor->AddBoxFixture((m_TilesWide - 2)* tileSize, (m_TilesHigh * 0.5f) * tileSize);
	m_DoorFixture = m_Actor->AddBoxFixture(tileSize * 1.5f, 2.5f * tileSize, true, apex::ApexMath::SFVec2fToB2Vec2(m_DoorPosition));

	b2Filter collisionFilter;
	collisionFilter.groupIndex = int(ActorID::BUILDING);
	collisionFilter.categoryBits = int(ActorID::PLAYER) | int(ActorID::AI_CHARACTER) | int(ActorID::BULLET);
	m_Actor->SetCollisionFilter(collisionFilter);
}

sf::Vector2f Building::GetBottomMiddlePoint()
{
	return m_Actor->GetPosition() + sf::Vector2f(0, m_TilesHigh * 16 * 0.75f / 2.0f);
}

b2Fixture* Building::GetDoorFixture()
{
	return m_DoorFixture;
}

size_t Building::GetIndex() const
{
	return m_Index;
}

void Building::SetDoorLocked(bool locked)
{
	m_DoorLocked = locked;
}

bool Building::IsDoorLocked() const
{
	return m_DoorLocked;
}
