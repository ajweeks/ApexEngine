
#include "Layer.h"
#include "PhysicsActor.h"
#include "Level.h"
#include "LevelTile.h"

Layer::Layer(Level* level, std::vector<int> tiles, TileSet* tileSet, std::map<int, bool> solidTileIDs,
	std::string name, bool visible, float opacity, Type type, int width, int height, ApexContactListener* contactListener) :
	m_Tiles(tiles), m_Name(name), m_Visible(visible), m_Opacity(opacity), m_Type(type),  m_Width(width), m_Height(height), m_TileSet(tileSet)
{
	const float tileSize = float(m_TileSet->m_TileSize);
	const int numTiles = m_Tiles.size();
	m_Verticies.setPrimitiveType(sf::Quads);
	m_Verticies.resize(numTiles * 4);
	for (int y = 0; y < m_Height; ++y)
	{
		for (int x = 0; x < m_Width; ++x)
		{
			const int tileIndex = (x + y * m_Width);
			sf::Vertex* currentQuad = &m_Verticies[tileIndex * 4];
			currentQuad[0].position = sf::Vector2f(x * tileSize, y * tileSize);
			currentQuad[1].position = sf::Vector2f((x+1) * tileSize, y * tileSize);
			currentQuad[2].position = sf::Vector2f((x+1) * tileSize, (y+1) * tileSize);
			currentQuad[3].position = sf::Vector2f(x * tileSize, (y+1) * tileSize);

			const int tileSrc = m_Tiles[tileIndex] - 1;
			const int textureTileWidth = m_TileSet->m_TilesWide;
			const int tileSrcX = tileSrc % textureTileWidth;
			const int tileSrcY = tileSrc / textureTileWidth;
			currentQuad[0].texCoords = sf::Vector2f(tileSrcX * tileSize, tileSrcY * tileSize);
			currentQuad[1].texCoords = sf::Vector2f((tileSrcX + 1) * tileSize, tileSrcY * tileSize);
			currentQuad[2].texCoords = sf::Vector2f((tileSrcX + 1) * tileSize, (tileSrcY + 1) * tileSize);
			currentQuad[3].texCoords = sf::Vector2f(tileSrcX * tileSize, (tileSrcY + 1) * tileSize);

			if (solidTileIDs[tileSrc])
			{
				LevelTile* newTile = new LevelTile(level, sf::Vector2f((x + 0.5f) * tileSize, (y + 0.5f) * tileSize), Entity::ActorID::WALL);
				newTile->GetPhysicsActor()->AddBoxFixture(tileSize, tileSize);
				newTile->GetPhysicsActor()->AddContactListener(contactListener);
				m_Entities.push_back(newTile);
			}
		}
	}
}

Layer::~Layer()
{
	delete m_TileSet;

	for (size_t i = 0; i < m_Entities.size(); i++)
	{
		if (m_Entities[i] != nullptr)
		{
			delete m_Entities[i];
		}
	}
}

void Layer::Tick(sf::Time elapsed)
{
}

void Layer::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	states.texture = m_TileSet->m_Texture;
	target.draw(m_Verticies, states);
}

int Layer::GetTileSize() const
{
	return m_TileSet->m_TileSize;
}

Layer::Type Layer::StringToType(std::string string)
{
	if (string.compare("tilelayer") == 0) return Type::TILE;
	else if (string.compare("imagelayer") == 0) return Type::IMAGE;
	else if (string.compare("objectgroup") == 0) return Type::OBJECT;
	return Type::NONE;
}
