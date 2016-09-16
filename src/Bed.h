#pragma once

#include "Item.h"

#include <SFML\Graphics\VertexArray.hpp>

class Bed : public Item
{
public:
	Bed(World& world, Map& map, sf::Vector2f position);
	virtual ~Bed();

	Bed(const Bed&) = delete;
	Bed& operator=(const Bed&) = delete;

	virtual void Tick(sf::Time elapsed) override;
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states) override;
	virtual sf::Vector2f GetBottomMiddlePoint() override;

	virtual void CreatePhysicsActor() override;

private:
	static const int TILES_WIDE = 2;
	static const int TILES_HIGH = 4;
	static const int TILE_X_OFFSET = 8;
	static const int TILE_Y_OFFSET = 4;

	sf::Texture* m_Texture;
	sf::VertexArray m_Verticies;

};

