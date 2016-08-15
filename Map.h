#pragma once

#include "ApexContactListener.h"
#include "Layer.h"

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

#include <string>
#include <vector>

class Layer;
class World;
struct TileSet;

class Map
{
public:
	Map();
	Map(World* world, std::string filePath, ApexContactListener* contactListener);
	virtual ~Map();

	void Create(World* world, std::string filePath, ApexContactListener* contactListener);

	Map(const Map&) = delete;
	Map& operator=(const Map&) = delete;

	void Tick(sf::Time elapsed);
	void DrawForeground(sf::RenderTarget& target, sf::RenderStates states);
	void DrawBackground(sf::RenderTarget& target, sf::RenderStates states);

	int GetTilesWide() const;
	int GetTilesHigh() const;
	int GetTileSize() const;

private:
	// Extra chars is filled with all characters after one past the begin string 
	//		eg.  StringBeginsWith("door_1", "door", e) -> e = "1"
	bool StringBeginsWith(const std::string& string, const std::string& begin, std::string& extraChars);

	int m_TilesWide;
	int m_TilesHigh;
	std::vector<Layer*> m_BackgroundLayers;
	std::vector<Layer*> m_ForegroundLayers;
	TileSet* m_TileSet = nullptr;

};
