
#include "Minimap.h"
#include "World.h"
#include "ApexMain.h"

#include <SFML\Graphics\RectangleShape.hpp>

const float Minimap::DEFAULT_SCALE = 0.25f;
const float Minimap::EDGE_WIDTH = 14.0f;
const float Minimap::MARGIN = 10.0f;

Minimap::Minimap(World* world, sf::Vector2f size, Location location) :
	m_World(world), m_Location(location)
{
	m_Bounds.width = size.x;
	m_Bounds.height = size.y;

	OnWindowResize(APEX->GetWindowSize());
}

Minimap::~Minimap()
{
}

void Minimap::Tick(sf::Time elapsed)
{
}

void Minimap::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	sf::RectangleShape border(sf::Vector2f(m_Bounds.width + EDGE_WIDTH * 2, m_Bounds.height + EDGE_WIDTH * 2));
	border.setPosition(m_Bounds.left, m_Bounds.top);
	border.setOutlineColor(sf::Color(5, 10, 5));
	border.setOutlineThickness(-2.0f);
	target.draw(border, states);

	states.transform.translate(388, -70)
		.scale(DEFAULT_SCALE, DEFAULT_SCALE, m_Bounds.left + m_Bounds.width / 2, m_Bounds.top + m_Bounds.height / 2);
	//m_World->DrawMap(target, states);
}

void Minimap::SetSize(sf::Vector2f size)
{
	m_Bounds.width = size.x;
	m_Bounds.height = size.y;

	// Update location
	OnWindowResize(APEX->GetWindowSize());
}

void Minimap::OnWindowResize(sf::Vector2u windowSize)
{
	if (m_Location == Location::NE || m_Location == Location::NW) m_Bounds.top = MARGIN;
	else m_Bounds.top = windowSize.y - m_Bounds.height - MARGIN - EDGE_WIDTH * 2;

	if (m_Location == Location::NW || m_Location == Location::SW) m_Bounds.left = MARGIN;
	else m_Bounds.left = windowSize.x - m_Bounds.width - MARGIN - EDGE_WIDTH * 2;
}
