
#include "Building.h"
#include "Map.h"

Building::Building(World* world, const std::string filePath, ApexContactListener* contactLister)
{
	m_Map = new Map(world, filePath, contactLister);
}

Building::~Building()
{
	delete m_Map;
}

void Building::Tick(sf::Time elapsed)
{
	m_Map->Tick(elapsed);
}

void Building::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	m_Map->DrawBackground(target, states);
	m_Map->DrawForeground(target, states);
}
