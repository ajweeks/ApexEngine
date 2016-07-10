
#include "Level.h"
#include "ApexMain.h"
#include "Map.h"
#include "ApexKeyboard.h"
#include "Camera.h"
#include "ApexAudio.h"
#include "ApexPauseScreen.h"

Level::Level()
{
	m_DebugOverlay = new ApexDebug();
	m_ShowingDebugOverlay = true;

	m_BulletManager = new BulletManager();
	m_Map = new Map(this, "resources/level/00/tiles_small.json");
	m_Width = m_Map->GetTilesWide() * m_Map->GetTileSize();
	m_Height = m_Map->GetTilesHigh() * m_Map->GetTileSize();
	m_Player = new Player(this);
	m_Camera = new Camera(sf::Vector2f(float(APEX->GetWindowSize().x), float(APEX->GetWindowSize().y)));
	m_Camera->SetZoom(2.0f);
	m_PauseScreen = new ApexPauseScreen(this);
	Reset();
}

Level::~Level()
{
	delete m_Map;
	delete m_Player;
	delete m_Camera;
	delete m_DebugOverlay;
	delete m_BulletManager;
	delete m_PauseScreen;
}

void Level::Reset()
{
	m_Player->Reset();
	m_BulletManager->Reset();
}

void Level::Tick(sf::Time elapsed)
{
	if (m_Paused)
	{
		m_PauseScreen->Tick(elapsed);
		return;
	}

	m_Map->Tick(elapsed);
	m_Player->Tick(elapsed);
	m_Camera->Tick(elapsed, this);
	m_BulletManager->Tick(elapsed);
	m_DebugOverlay->Tick(elapsed);
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

	if (m_Paused)
	{
		m_PauseScreen->Draw(target, sf::RenderStates::Default);
	}

	target.setView(currentView);
}

void Level::TogglePaused(bool pauseSounds)
{
	m_Paused = !m_Paused;
	APEX->SetPhysicsPaused(m_Paused);

	if (m_Paused)
	{
		m_PauseScreen->SetScreenShowing(ApexPauseScreen::Screen::MAIN);
	}

	if (pauseSounds) ApexAudio::SetAllPaused(m_Paused);
}

bool Level::IsPaused() const
{
	return m_Paused;
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

void Level::JoltCamera(float xAmount, float yAmount)
{
	m_Camera->Jolt(xAmount, yAmount);
}

void Level::SetScreenShake(float xScale, float yScale)
{
	m_Camera->Shake(xScale, yScale);
}
