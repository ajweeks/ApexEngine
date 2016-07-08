#pragma once

#include <SFML\Graphics\Texture.hpp>
#include "ApexMain.h"

struct TileSet
{
	TileSet(std::string imageFilePath, int tileSize, int margin, int spacing) :
		m_TileSize(tileSize), m_Margin(margin), m_Spacing(spacing)
	{
		m_Texture = new sf::Texture();
		if (!m_Texture->loadFromFile(imageFilePath))
		{
			OutputDebugString("Couldn't load image \"" + imageFilePath + "\"!\n");
		}
		m_TilesWide = m_Texture->getSize().x / m_TileSize;
		m_TilesHigh = m_Texture->getSize().y / m_TileSize;
	}

	virtual ~TileSet()
	{
		delete m_Texture;
	}

	sf::Texture* m_Texture;
	int m_TileSize;
	int m_TilesWide;
	int m_TilesHigh;
	int m_Margin;
	int m_Spacing;
};
