#pragma once

#include "Entity.h"

#include <SFML\Graphics\Texture.hpp>
#include <SFML\Graphics\VertexArray.hpp>

class ExitSensor : public Entity
{
public:
	ExitSensor(World& world, Map& map, sf::Vector2f position);
	virtual ~ExitSensor();

	ExitSensor(const ExitSensor&) = delete;
	ExitSensor& operator=(const ExitSensor&) = delete;

	virtual void Tick(sf::Time elapsed) override;
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states) override;

	virtual sf::Vector2f GetBottomMiddlePoint() override;
	virtual void CreatePhysicsActor() override;

private:
	static const int TILES_WIDE = 2;
	static const int TILES_HIGH = 1;
	static const int TILE_X_OFFSET = 3;
	static const int TILE_Y_OFFSET = 2;

	sf::Texture* m_Texture;
	sf::VertexArray m_Verticies;

};
