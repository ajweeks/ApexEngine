#pragma once

#include "JSON\json.hpp"
#include "ApexContactListener.h"

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

class World;
class Map;

class Building
{
public:
	Building(World* world, const std::string filePath, ApexContactListener* contactLister);
	virtual ~Building();

	Building(const Building&) = delete;
	Building& operator=(const Building&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

private:
	int m_Index;
	World* m_World = nullptr;
	Map* m_Map = nullptr;


};
