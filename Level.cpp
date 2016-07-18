
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

Level::Level() :
	ApexWindowListener()
{
	m_DebugOverlay = new ApexDebug();
	m_ShowingDebugOverlay = true;

	m_BulletManager = new BulletManager();
	m_Map = new Map(this, "resources/level/00/tiles_small.json", this);
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

	LoadShaders();

	m_Lights.push_back({ sf::Vector2f(250.0f, 100.0f), sf::Color(240, 240, 180), 30.0f, 25.0f, 1.0f });
	m_Lights.push_back({ sf::Vector2f(50.0f, 20.0f), sf::Color(140, 240, 200), 30.0f, 5.0f, 0.8f });

	m_LightmapTexture.create(windowSize.x, windowSize.y);
	m_LightmapSprite.setTexture(m_LightmapTexture.getTexture());

	Reset();
}

void Level::LoadShaders()
{
	if (!m_LightingShader.loadFromFile("resources/shaders/lighting.frag", sf::Shader::Fragment))
	{
		ApexOutputDebugString("\n\n\nCould not compile lighting shader\n\n\n\n");
	}
	const sf::Vector2u windowSize = APEX->GetWindowSize();
	m_LightingShader.setParameter("u_resolution", float(windowSize.x), float(windowSize.y));
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


	m_DebugOverlay->Tick(elapsed);

	const float time = APEX->GetTimeElapsed().asSeconds();
	m_Lights[0].opacity = ((sin(time) + 1.0f) / 8.0f + 0.75f);
	m_LightingShader.setParameter("u_opacity", m_Lights[0].opacity);
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


	DrawLighting(target, states);

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

void Level::DrawLighting(sf::RenderTarget& target, sf::RenderStates states)
{
	m_LightmapTexture.clear();

	const sf::Color ambientColor = sf::Color(195, 210, 222, 50);
	sf::Transform prevTransform = states.transform;

	const sf::Vector2f windowSize = static_cast<sf::Vector2f>(APEX->GetWindowSize());
	sf::RectangleShape screen(windowSize);
	screen.setFillColor(ambientColor);
	m_LightmapTexture.draw(screen);

	screen.setFillColor(sf::Color::Black);
	for (size_t i = 0; i < m_Lights.size(); i++)
	{
		m_LightingShader.setParameter("u_color", m_Lights[i].color);
		m_LightingShader.setParameter("u_radius", m_Lights[i].radius);
		m_LightingShader.setParameter("u_position", m_Lights[i].position);
		m_LightingShader.setParameter("u_opacity", m_Lights[i].opacity);
		m_LightingShader.setParameter("u_blur", m_Lights[i].blur);

		m_LightmapTexture.draw(screen, &m_LightingShader);
	}

	m_LightmapTexture.display();
	m_LightmapSprite.setTexture(m_LightmapTexture.getTexture());
	target.draw(m_LightmapSprite, sf::BlendMultiply);
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
	m_LightingShader.setParameter("u_resolution", float(windowSize.x), float(windowSize.y));
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
