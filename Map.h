#pragma once

#include <string>
#include <vector>
#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\RenderStates.hpp>

class Layer;
class Level;

class Map
{
public:
	Map();
	Map(Level* level, std::string filePath);
	virtual ~Map();

	void Create(Level* level, std::string filePath);

	Map(const Map&) = delete;
	Map& operator=(const Map&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

private:
	int m_TilesWide;
	int m_TilesHigh;
	std::vector<Layer*> m_Layers;

};

