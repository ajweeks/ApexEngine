
#include "Level.h"
#include "ApexMain.h"
#include "Map.h"
#include "ApexKeyboard.h"
#include "Camera.h"

Level::Level()
{
	m_DebugOverlay = new ApexDebug();
	m_BulletManager = new BulletManager();
	m_Map = new Map(this, "resources/level/00/tiles_small.json");
	m_Width = m_Map->GetTilesWide() * m_Map->GetTileSize();
	m_Height = m_Map->GetTilesHigh() * m_Map->GetTileSize();
	m_Player = new Player(this);
	m_Camera = new Camera(sf::Vector2f(float(APEX->GetWindowSize().x), float(APEX->GetWindowSize().y)));
	m_Camera->SetZoom(2.0f);
	Reset();
}

Level::~Level()
{
	delete m_Map;
	delete m_Player;
	delete m_Camera;
	delete m_DebugOverlay;
	delete m_BulletManager;
}

void Level::Reset()
{
	m_Player->Reset();
	m_BulletManager->Reset();
}

void Level::Tick(sf::Time elapsed)
{
	m_Map->Tick(elapsed);
	m_Player->Tick(elapsed);
	m_Camera->Tick(elapsed, this);
	m_BulletManager->Tick(elapsed);

	m_DebugOverlay->Tick(elapsed);
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
	const sf::View currentView = m_Camera->GetCurrentView();
	target.setView(currentView);
	
	m_Map->Draw(target, states);
	m_Player->Draw(target, states);
	m_BulletManager->Draw(target, states);

	if (m_ShowingDebugOverlay)
	{
		target.setView(target.getDefaultView());
		m_DebugOverlay->Draw(target, states);
	}

	target.setView(currentView);
}


unsigned int Level::GetWidth() const
{
	return m_Width;
}

unsigned int Level::GetHeight() const
{
	return m_Height;
}

Player* Level::GetPlayer()
{
	return m_Player;
}

BulletManager* Level::GetBulletManager()
{
	return m_BulletManager;
}

void Level::ToggleDebugOverlay()
{
	m_ShowingDebugOverlay = !m_ShowingDebugOverlay;
}

bool Level::IsShowingDebugOverlay() const
{
	return m_ShowingDebugOverlay;
}

sf::View Level::GetCurrentView() const
{
	return m_Camera->GetCurrentView();
}

void Level::SetScreenShake(float xScale, float yScale)
{
	m_Camera->Shake(xScale, yScale);
}
