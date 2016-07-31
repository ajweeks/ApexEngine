#pragma once

#include <SFML\Graphics\Texture.hpp>
#include "ApexMain.h"

struct TileSet
{
	TileSet(sf::Texture* texture, int tileSize, int margin, int spacing) :
		m_TileSize(tileSize), m_Margin(margin), m_Spacing(spacing)
	{
		m_Texture = texture;
		m_TilesWide = m_Texture->getSize().x / m_TileSize;
		m_TilesHigh = m_Texture->getSize().y / m_TileSize;
	}

	virtual ~TileSet()
	{
	}

	sf::Texture* m_Texture;
	int m_TileSize;
	int m_TilesWide;
	int m_TilesHigh;
	int m_Margin;
	int m_Spacing;
};
