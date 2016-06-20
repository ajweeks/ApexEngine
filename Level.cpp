
#include "Level.h"
#include "Player.h"
#include "Game.h"
#include "Map.h"
#include "ApexKeyboard.h"

Level::Level(Game* game) :
	WIDTH(96*32),
	HEIGHT(96*32),
	m_Game(game),
	m_Map("resources/level/00/tiles.json"),
	KeyListener(game)
{
	m_Player = new Player(this);

	Reset();
}

Level::~Level()
{
	delete m_Player;
}

void Level::Reset()
{
	m_Player->Reset();
	m_Size = sf::Vector2u(WIDTH, HEIGHT);
	m_View.reset(sf::FloatRect(0, 0, float(m_Game->GetWindowSize().x), float(m_Game->GetWindowSize().y)));
}

void Level::Tick(sf::Time elapsed)
{
	m_Map.Tick(elapsed);
	m_Player->Tick(elapsed);

	//const float dx = m_Player->GetPosition().x - m_View.getCenter().x;
	//const float dy = m_Player->getPosition().y - m_View.getCenter().y;
	//if (dx > 500) m_View.move(dx - 500, 0);
	//else if (dx < -500) m_View.move(dx + 500, 0);
	//if (dy > 360) m_View.move(0, dy - 360);
	//if (dy < -360) m_View.move(0, dy + 360);

	const sf::Vector2f viewSize = m_View.getSize();
	if (m_View.getCenter().x - viewSize.x / 2.0f < 0)
		m_View.setCenter(viewSize.x / 2.0f, m_View.getCenter().y);
	if (m_View.getCenter().x + viewSize.x / 2.0f > WIDTH)
		m_View.setCenter(WIDTH - viewSize.x / 2.0f, m_View.getCenter().y);

	if (m_View.getCenter().y - viewSize.y / 2.0f < 0)
		m_View.setCenter(m_View.getCenter().x, viewSize.y / 2.0f);
	if (m_View.getCenter().y + viewSize.y / 2.0f > HEIGHT)
		m_View.setCenter(m_View.getCenter().x, HEIGHT - viewSize.y / 2.0f);

	CheckCollisions();

	m_DebugOverlay.Tick(elapsed, m_Game);
}

void Level::CheckCollisions()
{

}

bool Level::IsPointInPolygon(std::vector<sf::Vector2i> points, sf::Vector2f point) 
{
	bool c = false;
	int l = points.size();
	int j = l - 1;
	for (int i = -1; ++i < l; j = i)
	{
		// Magic algorithm
		if (((points[i].y <= point.y && point.y < points[j].y) || (points[j].y <= point.y && point.y < points[i].y))
			&& (point.x < (points[j].x - points[i].x) * (point.y - points[i].y) / (points[j].y - points[i].y) + points[i].x))
		{
			c = !c;
		}
	}
	return c;
}

void Level::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	target.setView(m_View);
	
	m_Map.Draw(target, states);
	m_Player->Draw(target, states);

	if (m_IsShowingDebugOverlay) m_DebugOverlay.Draw(target);
}

unsigned int Level::GetWidth() const
{
	return m_Size.x;
}

unsigned int Level::GetHeight() const
{
	return m_Size.y;
}

void Level::OnKeyPress(sf::Event::KeyEvent keyEvent)
{
	switch (keyEvent.code)
	{
	case sf::Keyboard::F9:
	{
		// Prevent multiple calls from a press and hold
		if (ApexKeyboard::IsKeyPressed(keyEvent.code))
		{
			m_IsShowingDebugOverlay = !m_IsShowingDebugOverlay;
		}
	} break;
	}
}

void Level::OnKeyRelease(sf::Event::KeyEvent keyEvent)
{
}

Player* Level::GetPlayer()
{
	return m_Player;
}
