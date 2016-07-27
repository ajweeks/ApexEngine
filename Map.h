#pragma once

#include "ApexContactListener.h"

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

#include <string>
#include <vector>

class Layer;
class Level;
struct TileSet;

class Map
{
public:
	Map();
	Map(Level* level, std::string filePath, ApexContactListener* contactListener);
	virtual ~Map();

	void Create(Level* level, std::string filePath, ApexContactListener* contactListener);

	Map(const Map&) = delete;
	Map& operator=(const Map&) = delete;

	void Tick(sf::Time elapsed);
	void DrawForeground(sf::RenderTarget& target, sf::RenderStates states);
	void DrawBackground(sf::RenderTarget& target, sf::RenderStates states);

	int GetTilesWide() const;
	int GetTilesHigh() const;
	int GetTileSize() const;

private:
	int m_TilesWide;
	int m_TilesHigh;
	std::vector<Layer*> m_BackgroundLayers;
	std::vector<Layer*> m_ForegroundLayers;
	TileSet* m_TileSet = nullptr;

};
