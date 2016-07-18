#pragma once

#include "ApexContactListener.h"

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

#include <string>
#include <vector>

class Layer;
class Level;

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
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

	int GetTilesWide() const;
	int GetTilesHigh() const;
	int GetTileSize() const;

private:
	int m_TilesWide;
	int m_TilesHigh;
	std::vector<Layer*> m_Layers;

};

