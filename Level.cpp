
#include "Level.h"
#include "ApexMain.h"
#include "Map.h"
#include "ApexKeyboard.h"
#include "Camera.h"
#include "ApexAudio.h"
#include "ApexPauseScreen.h"
#include "Mob.h"
#include "Sheep.h"
#include "PhysicsActor.h"
#include "Entity.h"
#include "Minimap.h"

Level::Level(int levelIndex) :
	ApexWindowListener(),
	m_LevelIndex(levelIndex),
	m_LightManager(levelIndex, this)
{
	m_DebugOverlay = new ApexDebug();
	m_ShowingDebugOverlay = true;

	m_BulletManager = new BulletManager();
	m_Map = new Map(this, "resources/level/" + std::to_string(levelIndex) + "/tiles_small.json", this);
	m_Width = m_Map->GetTilesWide() * m_Map->GetTileSize();
	m_Height = m_Map->GetTilesHigh() * m_Map->GetTileSize();
	m_Player = new Player(this);
	const sf::Vector2u windowSize = APEX->GetWindowSize();
	const float aspectRatio = float(windowSize.x / windowSize.y);
	m_Camera = new Camera(sf::Vector2f(float(windowSize.x), float(windowSize.y)));
	m_Camera->SetZoom(2.0f);
	m_PauseScreen = new ApexPauseScreen(this);
	const float minimapWidth = 200.0f;
	m_Minimap = new Minimap(this, sf::Vector2f(minimapWidth, minimapWidth / aspectRatio));

	Reset();
}

void Level::LoadShaders()
{
	m_LightManager.LoadShader();
}

void Level::LoadLights()
{
	m_LightManager.LoadLights();
}

void Level::ToggleLightingEditor()
{
	m_LightManager.ToggleShowingEditor();
}

Level::~Level()
{
	delete m_Map;
	delete m_Player;
	delete m_Camera;
	delete m_DebugOverlay;
	delete m_BulletManager;
	delete m_PauseScreen;
	delete m_Minimap;

	for (size_t i = 0; i < m_Mobs.size(); i++)
	{
		delete m_Mobs[i];
	}
	m_Mobs.clear();
}

void Level::Reset()
{
	m_Player->Reset();
	m_BulletManager->Reset();

	for (size_t i = 0; i < m_Mobs.size(); i++)
	{
		delete m_Mobs[i];
	}
	m_Mobs.clear();
	m_Mobs.push_back(new Sheep(this, sf::Vector2f(400, 50)));
	m_Mobs.push_back(new Sheep(this, sf::Vector2f(430, 510)));
	m_Mobs.push_back(new Sheep(this, sf::Vector2f(690, 240)));
	m_Mobs.push_back(new Sheep(this, sf::Vector2f(910, 580)));
}

void Level::Tick(sf::Time elapsed)
{
	if (m_Paused)
	{
		m_PauseScreen->Tick(elapsed);
		return;
	}

	m_Map->Tick(elapsed);
	m_Minimap->Tick(elapsed);
	m_Player->Tick(elapsed);
	m_Camera->Tick(elapsed, this);
	m_BulletManager->Tick(elapsed);

	for (size_t i = 0; i < m_Mobs.size(); i++)
	{
		if (m_Mobs[i] != nullptr)
		{
			m_Mobs[i]->Tick(elapsed);
		}
	}

	m_ParticleManager.Tick(elapsed);

	m_DebugOverlay->Tick(elapsed);

	m_LightManager.Tick(elapsed);
}

void Level::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	sf::View cameraView = m_Camera->GetCurrentView();
	target.setView(cameraView);
	
	// Elements in world
	DrawMap(target, states);

	m_BulletManager->Draw(target, states);
	m_Player->Draw(target, states);

	for (size_t i = 0; i < m_Mobs.size(); i++)
	{
		if (m_Mobs[i] != nullptr)
		{
			m_Mobs[i]->Draw(target, states);
		}
	}

	m_ParticleManager.Draw(target, states);

	m_LightManager.Draw(target, states);

	// Static elements
	target.setView(target.getDefaultView());

	m_Minimap->Draw(target, states);

	if (m_ShowingDebugOverlay)
	{
		m_DebugOverlay->Draw(target, states);
	}

	if (m_Paused)
	{
		m_PauseScreen->Draw(target, sf::RenderStates::Default);
	}

	target.setView(cameraView);
}

void Level::DrawMap(sf::RenderTarget& target, sf::RenderStates states)
{
	m_Map->Draw(target, states);
}

void Level::TogglePaused(bool pauseSounds)
{
	m_Paused = !m_Paused;
	APEX->SetPhysicsPaused(m_Paused);
	m_DebugOverlay->ClearAllInput();

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

void Level::AddParticle(ApexParticle* spriteSheet)
{
	m_ParticleManager.AddParticle(spriteSheet);
}

void Level::RemoveMob(Mob* mob)
{
	for (size_t i = 0; i < m_Mobs.size(); i++)
	{
		if (m_Mobs[i] == mob)
		{
			delete m_Mobs[i];
			m_Mobs[i] = nullptr;
			return;
		}
	}
}

void Level::BeginContact(PhysicsActor* thisActor, PhysicsActor* otherActor)
{
}

void Level::EndContact(PhysicsActor* thisActor, PhysicsActor* otherActor)
{
}

void Level::PreSolve(PhysicsActor* thisActor, PhysicsActor* otherActor, bool & enableContact)
{
}

void Level::OnWindowResize(sf::Vector2u windowSize)
{
	m_LightManager.OnWindowResize(windowSize);
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
	m_DebugOverlay->ClearAllInput();
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
