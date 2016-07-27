
#include "Map.h"
#include "Layer.h"
#include "TileSet.h"

#include <JSON\json.hpp>

#include <fstream>

using json = nlohmann::json;

Map::Map()
{
}

Map::Map(Level* level, std::string filePath, ApexContactListener* contactListener)
{
	Create(level, filePath, contactListener);
}

void Map::Create(Level* level, std::string filePath, ApexContactListener* contactListener)
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
	std::string tileSetImagePath = tileSets[0]["image"];
	int tileSetTileSize = tileSets[0]["tilewidth"];
	int tileSetSpacing = tileSets[0]["spacing"];
	int tileSetMargin = tileSets[0]["margin"];
	int tileCount = tileSets[0]["tilecount"];
	json tilePropertiesObject = tileSets[0]["tileproperties"];
	std::map<int, bool> solidTileIDs;
	for (int i = 0; i < tileCount; ++i)
	{
		if (tilePropertiesObject.find(std::to_string(i)) != tilePropertiesObject.end())
		{
			json currentTile = tilePropertiesObject[std::to_string(i)];
			solidTileIDs[i] = currentTile["solid"].get<bool>();
		}
	}
	m_TileSet = new TileSet(tileSetImagePath, tileSetTileSize, tileSetMargin, tileSetSpacing);

	json layers = tileMap["layers"];
	for (json::iterator i = layers.begin(); i != layers.end(); ++i)
	{
		json currentLayer = *i;
		if (currentLayer["type"].get<std::string>().compare("objectgroup") == 0)
		{
			// Skip object layers for now
			continue;
		}

		std::vector<int> layerData = currentLayer["data"];
		std::string layerName = currentLayer["name"];
		Layer::Type layerType = Layer::StringToType(currentLayer["type"]);
		bool layerVisible = currentLayer["visible"];
		float layerOpacity = currentLayer["opacity"];
		int layerWidth = currentLayer["width"];
		int layerHeight = currentLayer["height"];

		Layer* newLayer = new Layer(level, layerData, m_TileSet, solidTileIDs, layerName, layerVisible,
			layerOpacity, layerType, layerWidth, layerHeight, contactListener);

		if (layerName.compare("foreground") == 0) m_ForegroundLayers.push_back(newLayer);
		else m_BackgroundLayers.push_back(newLayer);
	}
	std::string orientation = tileMap["orientation"];
	if (orientation.compare("orthogonal") != 0)
	{
		std::cout << "Non-orthogonal map passed in! The type specified was \"" << orientation << "\"!" << std::endl;
		return;
	}
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

int Map::GetTilesWide() const
{
	return m_TilesWide;
}

int Map::GetTilesHigh() const
{
	return m_TilesHigh;
}
