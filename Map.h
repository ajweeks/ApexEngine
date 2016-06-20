#pragma once

#include <string>
#include <vector>
#include <SFML\Graphics.hpp>

class Layer;

class Map
{
public:
	Map(std::string filePath);
	virtual ~Map();

	Map(const Map&) = delete;
	Map& operator=(const Map&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

private:
	int m_TilesWide;
	int m_TilesHigh;
	std::vector<Layer*> m_Layers;

};

