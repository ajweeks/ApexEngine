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

	virtual void CreatePhysicsActor() override;

private:
	static const int WIDTH = 3;
	static const int HEIGHT = 7;

	sf::Texture* m_Texture;
	sf::VertexArray m_Verticies;

};

