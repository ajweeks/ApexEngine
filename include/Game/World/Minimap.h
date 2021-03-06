#pragma once

#include "Apex\WindowListener.h"

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\Rect.hpp>

class World;

class Minimap : public apex::WindowListener
{
public:
	enum class Location // Which corner of the window we're in
	{
		NE, NW, SE, SW
	};

	Minimap(World& world, sf::Vector2f size, Location location = Location::NE);
	virtual ~Minimap();

	Minimap(const Minimap&) = delete;
	Minimap& operator=(const Minimap&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

	void SetSize(sf::Vector2f size);

	virtual void OnWindowResize(sf::Vector2u windowSize) override;

private:
	static const float DEFAULT_SCALE;
	static const float EDGE_WIDTH;
	static const float MARGIN;

	World& m_World;

	sf::FloatRect m_Bounds;
	Location m_Location;

};
