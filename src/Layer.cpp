
#include "Layer.h"
#include "PhysicsActor.h"
#include "Building.h"

Layer::Layer(World* world, std::vector<Tile*> tiles, TileSet* tileSet,
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

			const int tileSrc = m_Tiles[tileIndex]->GetID() - 1;
			if (tileSrc == -1)
			{
				currentQuad[0].color = sf::Color::Transparent;
				currentQuad[1].color = sf::Color::Transparent;
				currentQuad[2].color = sf::Color::Transparent;
				currentQuad[3].color = sf::Color::Transparent;
			}
			else
			{
				const int textureTileWidth = m_TileSet->m_TilesWide;
				const int tileSrcX = tileSrc % textureTileWidth;
				const int tileSrcY = tileSrc / textureTileWidth;
				currentQuad[0].texCoords = sf::Vector2f(tileSrcX * tileSize, tileSrcY * tileSize);
				currentQuad[1].texCoords = sf::Vector2f((tileSrcX + 1) * tileSize, tileSrcY * tileSize);
				currentQuad[2].texCoords = sf::Vector2f((tileSrcX + 1) * tileSize, (tileSrcY + 1) * tileSize);
				currentQuad[3].texCoords = sf::Vector2f(tileSrcX * tileSize, (tileSrcY + 1) * tileSize);

				if (m_Tiles[tileIndex]->IsSolid())
				{
					PhysicsActor* newActor = new PhysicsActor(sf::Vector2f((x + 0.5f) * tileSize, (y + 0.5f) * tileSize), b2BodyType::b2_staticBody);
					newActor->AddBoxFixture(tileSize, tileSize);
					newActor->SetUserPointer(m_Tiles[tileIndex]);
					newActor->AddContactListener(contactListener);

					b2Filter collisionFilter;
					if (m_Tiles[tileIndex]->IsDoorTile())
					{
						collisionFilter.categoryBits = ActorID::DOOR;
						newActor->SetUserData(ActorID::DOOR);
					}
					else
					{
						collisionFilter.categoryBits = ActorID::WALL;
						newActor->SetUserData(ActorID::WALL);
					}
					collisionFilter.maskBits = ActorID::BULLET | ActorID::PLAYER | ActorID::SHEEP;
					newActor->SetCollisionFilter(collisionFilter);

					m_Tiles[tileIndex]->SetPhysicsActor(newActor);
				}
			}
		}
	}
}

Layer::~Layer()
{
	for (size_t i = 0; i < m_Tiles.size(); ++i)
	{
		delete m_Tiles[i];
	}
	m_Tiles.clear();
}

// TODO: Finish implementing this
//void Layer::CreatePhysicsActors()
//{
//	/* -1 if we haven't merged this actor yet, 0 if no actor, else the index of the actor
//	eg.
//		1, 1, 1, 0, 2, 2,-1
//		1, 1, 1, 0, 0,-1,-1
//		1, 1, 1, 3, 3, 3,-1
//		0, 4, 4, 3, 3, 3, 0
//	*/
//
//	/*
//	The basic idea of this algorithm is to take a solid tile and see what the largest rectangle with a top left
//	corner of the current tile
//	*/
//	std::vector<int> m_ActorIndicies(m_Tiles.size(), -1);
//	for (size_t y = 0; y < m_TileSet->m_TilesHigh; ++y)
//	{
//		for (size_t x = 0; x < m_TileSet->m_TilesWide; ++x)
//		{
//			const int index = y * m_TileSet->m_TilesWide + x;
//
//			if (m_ActorIndicies[index] == -1)
//			{
//				int largestRectSize = 1;
//				sf::FloatRect largestRectBounds(x, y, 1, 1);
//
//				sf::FloatRect currentRectBounds(x, y, 1, 1);
//				for (size_t yy = y; yy < m_TileSet->m_TilesHigh; ++yy)
//				{
//					for (size_t xx = x; xx < m_TileSet->m_TilesWide; ++xx)
//					{
//						const int innerIndex = yy * m_TileSet->m_TilesWide + xx;
//						const int newWidth = xx - x + 1;
//						if (m_Tiles[innerIndex] != 0)
//						{
//							currentRectBounds.height = yy - y + 1;
//						}
//						else
//						{
//							if (newWidth - 1 < currentRectBounds.width) 
//							{
//								currentRectBounds.width = newWidth - 1;
//								yy = m_TileSet->m_TilesHigh;
//							}
//							else if (yy == y)
//							{
//								yy = m_TileSet->m_TilesHigh;
//							}
//							break;
//						}
//
//						int currentRectSize = currentRectBounds.width * currentRectBounds.height;
//						if (currentRectSize > largestRectSize)
//						{
//							largestRectSize = currentRectSize;
//							largestRectBounds = currentRectBounds;
//						}
//					}
//				}
//
//			}
//		}
//	}
//}

void Layer::Tick(sf::Time elapsed)
{
}

void Layer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (m_Visible)
	{
		states.texture = m_TileSet->m_Texture;
		target.draw(m_Verticies, states);
	}
}

int Layer::GetTileSize() const
{
	return m_TileSet->m_TileSize;
}

const std::string& Layer::GetName() const
{
	return m_Name;
}

Layer::Type Layer::ParseLayerTypeString(std::string string)
{
	if (string.compare("tilelayer") == 0) return Type::TILE;
	else if (string.compare("imagelayer") == 0) return Type::IMAGE;
	else if (string.compare("objectgroup") == 0) return Type::OBJECT;
	return Type::NONE;
}
