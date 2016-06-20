#pragma once

#include <vector>
#include <SFML\Graphics.hpp>
#include "TileSet.h"

class Layer : public sf::Drawable
{
public:
	enum class Type
	{
		TILE, OBJECT, IMAGE, NONE
	};

	Layer(std::vector<int> tiles, TileSet* tileSet, std::string name, bool visible, float opacity, Type type, int width, int height);
	virtual ~Layer();

	Layer(const Layer&) = delete;
	Layer& operator=(const Layer&) = delete;

	void Tick(sf::Time elapsed);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	static Type StringToType(std::string string);

private:
	std::vector<int> m_Tiles;
	sf::VertexArray m_Verticies;

	std::string m_Name;
	bool m_Visible;
	int m_Width;
	int m_Height;
	Type m_Type;
	float m_Opacity;
	TileSet* m_TileSet;

	sf::Time m_Elapsed;
};
