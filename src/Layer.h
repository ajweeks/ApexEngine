#pragma once

#include "TileSet.h"
#include "ApexContactListener.h"
#include "Tile.h"

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\VertexArray.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

#include <vector>
#include <string>
#include <map>

class World;
class LevelTile;

class Layer
{
public:
	enum class Type
	{
		TILE, OBJECT, IMAGE, NONE
	};

	Layer(World* world, std::vector<Tile*> tiles, TileSet* tileSet,
		std::string name, bool visible, float opacity, Type type, int width, int height);
	virtual ~Layer();

	Layer(const Layer&) = delete;
	Layer& operator=(const Layer&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

	void CreatePhysicsActors(ApexContactListener* contactListener);
	void DestroyPhysicsActors();

	int GetTileSize() const;
	const std::string& GetName() const;

	static Type ParseLayerTypeString(std::string layerTypeString);

private:
	World* m_World;

	std::vector<Tile*> m_Tiles;
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
