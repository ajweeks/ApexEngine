
#include "Map.h"
#include "Layer.h"
#include "TileSet.h"
#include "TextureManager.h"
#include "Tile.h"

#include <JSON\json.hpp>

#include <fstream>

using json = nlohmann::json;

Map::Map()
{
}

Map::Map(World* world, std::string filePath, ApexContactListener* contactListener)
{
	Create(world, filePath, contactListener);
}

void Map::Create(World* world, std::string filePath, ApexContactListener* contactListener)
{
	std::ifstream fileInStream;

	fileInStream.open(filePath);

	if (!fileInStream)
	{
		std::cout << "Couldn't find file \"" << filePath << "\"!" << std::endl;
		return;
	}

	std::string line;
	std::stringstream wholeFileStringStream;
	while (!fileInStream.eof())
	{
		std::getline(fileInStream, line);
		wholeFileStringStream << line;
	}
	fileInStream.close();

	std::string wholeFileString = wholeFileStringStream.str();
	wholeFileString.erase(std::remove_if(wholeFileString.begin(), wholeFileString.end(), isspace), wholeFileString.end());

	json tileMap = json::parse(wholeFileString);

	if (tileMap.empty())
	{
		std::cout << "File \"" << filePath << "\" was empty!" << std::endl;
		return;
	}

	m_TilesWide = tileMap["width"];
	m_TilesHigh = tileMap["height"];

	std::vector<json> tileSets = tileMap["tilesets"];
	if (tileSets.size() != 1)
	{
		std::cout << "More than one tile set in map! Please only use one!" << std::endl;
		return;
	}
	const int tileSetTileSize = tileSets[0]["tilewidth"];
	const int tileSetSpacing = tileSets[0]["spacing"];
	const int tileSetMargin = tileSets[0]["margin"];
	const int tileCount = tileSets[0]["tilecount"];
	json tileProperties = tileSets[0]["tileproperties"];

	std::vector<Tile*> tileAtlas;
	tileAtlas.reserve(tileCount);
	for (int i = 0; i < tileCount; ++i)
	{
		const int ID = i -1;
		int doorID = -1;
		bool solid = false;
		const std::string tileIndexString = std::to_string(ID);
		if (tileProperties.find(tileIndexString) != tileProperties.end())
		{
			const json currentTile = tileProperties[tileIndexString];
			if (currentTile.find("solid") != currentTile.end())
			{
				solid = currentTile["solid"].get<bool>();
			}
			if (currentTile.find("id") != currentTile.end())
			{
				const std::string tileIDString = currentTile["id"].get<std::string>();
				std::string extraChars;
				if (StringBeginsWith(tileIDString, "door", extraChars))
				{
					doorID = stoi(extraChars);
				}
				if (StringBeginsWith(tileIDString, "building", extraChars))
				{
					// TODO: "Inform" tiles that they are part of a building (?)
					//int buildingID = stoi(extraChars);
				}
				if (StringBeginsWith(tileIDString, "bed", extraChars))
				{
					// TODO: "Inform" tiles that they are part of a bed (?)
				}
			}
		}
		Tile* tile = new Tile(ID, solid, doorID);
		tileAtlas.push_back(tile);
	}

	m_TileSet = new TileSet(TextureManager::GetTexture(TextureManager::GENERAL_TILES), tileSetTileSize, tileSetMargin, tileSetSpacing);

	json layers = tileMap["layers"];
	for (json::iterator i = layers.begin(); i != layers.end(); ++i)
	{
		json currentLayer = *i;
		const std::string layerTypeString = currentLayer["type"].get<std::string>();
		const Layer::Type layerType = Layer::ParseLayerTypeString(layerTypeString);
		const std::string layerName = currentLayer["name"];
		const bool layerVisible = currentLayer["visible"];
		const float layerOpacity = currentLayer["opacity"];
		const int layerWidth = currentLayer["width"];
		const int layerHeight = currentLayer["height"];

		switch (layerType)
		{
		case Layer::Type::TILE:
		{
			const std::vector<int> tileIDs = currentLayer["data"];
			std::vector<Tile*> tiles;
			tiles.reserve(tileIDs.size());
			for (size_t i = 0; i < tileIDs.size(); ++i)
			{
				const int id = tileIDs[i];
				Tile* tile = new Tile(id, tileAtlas[id]->IsSolid(), tileAtlas[id]->GetDoorID());
				tiles.push_back(tile);
			}

			Layer* newLayer = new Layer(world, tiles, m_TileSet, layerName, layerVisible,
				layerOpacity, layerType, layerWidth, layerHeight, contactListener);

			if (layerName.compare("foreground") == 0) m_ForegroundLayers.push_back(newLayer);
			else m_BackgroundLayers.push_back(newLayer);
		} break;
		case Layer::Type::OBJECT:
		{
			// Skip image layers for now
		} break;
		case Layer::Type::IMAGE:
		{
			// Skip object layers for now
		}
		case Layer::Type::NONE:
		default:
		{
		} break;
		}
	}

	std::string orientation = tileMap["orientation"];
	if (orientation.compare("orthogonal") != 0)
	{
		std::cout << "Non-orthogonal map passed in! The type specified was \"" << orientation << "\"!" << std::endl;
		return;
	}

	for (size_t i = 0; i < tileAtlas.size(); ++i)
	{
		delete tileAtlas[i];
	}
	tileAtlas.clear();
}

Map::~Map()
{
	for (size_t i = 0; i < m_BackgroundLayers.size(); ++i)
	{
		delete m_BackgroundLayers[i];
	}
	for (size_t i = 0; i < m_ForegroundLayers.size(); ++i)
	{
		delete m_ForegroundLayers[i];
	}

	delete m_TileSet;
}

void Map::Tick(sf::Time elapsed)
{
	for (size_t i = 0; i < m_BackgroundLayers.size(); ++i)
	{
		m_BackgroundLayers[i]->Tick(elapsed);
	}
	for (size_t i = 0; i < m_ForegroundLayers.size(); ++i)
	{
		m_ForegroundLayers[i]->Tick(elapsed);
	}
}

void Map::DrawForeground(sf::RenderTarget& target, sf::RenderStates states)
{
	for (size_t i = 0; i < m_ForegroundLayers.size(); ++i)
	{
		m_ForegroundLayers[i]->draw(target, states);
	}
}

void Map::DrawBackground(sf::RenderTarget& target, sf::RenderStates states)
{
	for (size_t i = 0; i < m_BackgroundLayers.size(); ++i)
	{
		m_BackgroundLayers[i]->draw(target, states);
	}
}

int Map::GetTileSize() const
{
	return m_TileSet->m_TileSize;
}

bool Map::StringBeginsWith(const std::string& string, const std::string& begin, std::string& extraChars)
{
	if (string.length() >= begin.length() &&
		string.substr(0, begin.length()).compare(begin) == 0)
	{
		if (string.length() > begin.length() + 1)
		{
			extraChars = string.substr(begin.length() + 1);
		}
		return true;
	}
	return false;
}

int Map::GetTilesWide() const
{
	return m_TilesWide;
}

int Map::GetTilesHigh() const
{
	return m_TilesHigh;
}
