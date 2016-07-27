
#include "Level.h"
#include "ApexMain.h"
#include "Map.h"
#include "ApexKeyboard.h"
#include "Camera.h"
#include "ApexAudio.h"
#include "ApexPauseScreen.h"
#include "PhysicsActor.h"
#include "Minimap.h"
#include "Mob.h"
#include "Sheep.h"
#include "Entity.h"
#include "Item.h"
#include "AmmoDrop.h"
#include "HUD.h"
#include "Interactable.h"

Level::Level(int levelIndex) :
	ApexWindowListener(),
	m_LevelIndex(levelIndex),
	m_LightManager(levelIndex, this)
{
	m_DebugOverlay = new ApexDebug();
	m_ShowingDebugOverlay = false;

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

	m_HUD = new HUD(this);
	
	Reset();
}

void Level::LoadShaders()
{
	m_LightManager.LoadShader();
}

void Level::LoadLights()
{
	m_LightManager.LoadLightData();
}
	if (!m_OutlinedSpriteShader.loadFromFile("resources/shaders/outline_sprite.frag", sf::Shader::Fragment))
	{
		ApexOutputDebugString("Could not either load or compile color-sprite.frag\n");
	}
	m_OutlinedSpriteShader.setParameter("u_color", sf::Color(255, 255, 255));

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
	delete m_HUD;

	for (size_t i = 0; i < m_Mobs.size(); i++)
	{
		if (m_Mobs[i] != nullptr)
		{
			delete m_Mobs[i];
		}
	}
	m_Mobs.clear();

	for (size_t i = 0; i < m_Items.size(); i++)
	{
		if (m_Items[i] != nullptr)
		{
			delete m_Items[i];
		}
	}
	m_Items.clear();

	for (size_t i = 0; i < m_ItemsToBeRemoved.size(); ++i)
	{
		delete m_ItemsToBeRemoved[i];
	}
	m_ItemsToBeRemoved.clear();
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

	for (size_t i = 0; i < m_Items.size(); i++)
	{
		delete m_Items[i];
	}
	m_Items.clear();
	m_Items.push_back(new AmmoDrop(this, sf::Vector2f(250, 80), 100));
	m_Items.push_back(new AmmoDrop(this, sf::Vector2f(550, 110), 100));
	m_Items.push_back(new AmmoDrop(this, sf::Vector2f(150, 610), 100));
}

void Level::Tick(sf::Time elapsed)
{
	if (m_Paused)
	{
		m_PauseScreen->Tick(elapsed);
		return;
	}

	for (size_t i = 0; i < m_ItemsToBeRemoved.size(); ++i)
	{
		delete m_ItemsToBeRemoved[i];
	}
	m_ItemsToBeRemoved.clear();

	m_Map->Tick(elapsed);
	m_Minimap->Tick(elapsed);
	m_Player->Tick(elapsed);
	m_Camera->Tick(elapsed, this);
	m_BulletManager->Tick(elapsed);
	m_HUD->Tick(elapsed);

	for (size_t i = 0; i < m_Mobs.size(); i++)
	{
		if (m_Mobs[i] != nullptr)
		{
			m_Mobs[i]->Tick(elapsed);
		}
	}

	for (size_t i = 0; i < m_Items.size(); i++)
	{
		if (m_Items[i] != nullptr)
		{
			m_Items[i]->Tick(elapsed);
		}
	}

	m_ParticleManager.Tick(elapsed);
	if (m_ShowingDebugOverlay) m_DebugOverlay->Tick(elapsed);
	m_LightManager.Tick(elapsed);

	const float minDist = 28.0f;
	float distanceToNearestEntity;
	Entity* nearestEntity = GetNearestEntityTo(m_Player, distanceToNearestEntity);
	if (distanceToNearestEntity != -1.0f && distanceToNearestEntity <= minDist)
	{
		m_HighlightedEntity = nearestEntity;
	}
	else
	{
		m_HighlightedEntity = nullptr;
	}

#if 0
	// Rainbow outline
	const float time = APEX->GetTimeElapsed().asSeconds();
	const sf::Uint8 outlineR = sf::Uint8((sin(time * 15.0f + 1.2f) + 1.0f) * 127);
	const sf::Uint8 outlineG = sf::Uint8((cos(time * 20.5f) + 1.0f) * 127);
	const sf::Uint8 outlineB = sf::Uint8((sin(time * 12.0f + 0.5f) + 1.0f) * 127);
	m_OutlinedSpriteShader.setParameter("u_color", sf::Color(outlineR, outlineG, outlineB));
#endif
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
		if (m_Mobs[i] != nullptr && m_Mobs[i] != m_HighlightedEntity)
		{
			m_Mobs[i]->Draw(target, states);
		}
	}

	for (size_t i = 0; i < m_Items.size(); i++)
	{
		if (m_Items[i] != nullptr && m_Items[i] != m_HighlightedEntity)
		{
			m_Items[i]->Draw(target, states);
		}
	}

	if (m_HighlightedEntity != nullptr)
	{
		states.shader = &m_OutlinedSpriteShader;
		m_HighlightedEntity->Draw(target, states);
		states.transform = states.Default.transform;
		states.shader = states.Default.shader;
	}

	m_ParticleManager.Draw(target, states);

	m_LightManager.Draw(target, states);

	// Static elements
	target.setView(target.getDefaultView());

	m_Minimap->Draw(target, states);

	m_HUD->Draw(target, states);

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

void Level::InteractWithHighlightedItem()
{
	if (m_HighlightedEntity != nullptr)
	{
		Interactable* interactable = dynamic_cast<Interactable*>(m_HighlightedEntity);
		if (interactable) // If this item is interactable
		{
			interactable->Interact();
		}
	}
}

Entity* Level::GetNearestEntityTo(Entity* sourceEntity, float& distance)
{
	Entity* nearestSoFar = nullptr;
	distance = -1.0f;

	const sf::Vector2f sourcePos = sourceEntity->GetPhysicsActor()->GetPosition();
	for (size_t i = 0; i < m_Mobs.size(); ++i)
	{
		const sf::Vector2f otherPos = m_Mobs[i]->GetPhysicsActor()->GetPosition();
		const float currentDistance = ApexMath::Distance(sourcePos, otherPos);

		if (distance == -1.0f || currentDistance < distance)
		{
			distance = currentDistance;
			nearestSoFar = m_Mobs[i];
		}
	}
	for (size_t i = 0; i < m_Items.size(); ++i)
	{
		const sf::Vector2f otherPos = m_Items[i]->GetPhysicsActor()->GetPosition();
		const float currentDistance = ApexMath::Distance(sourcePos, otherPos);

		if (distance == -1.0f || currentDistance < distance)
		{
			distance = currentDistance;
			nearestSoFar = m_Items[i];
		}
	}

	return nearestSoFar;
}

void Level::AddParticle(ApexParticle* spriteSheet)
{
	m_ParticleManager.AddParticle(spriteSheet);
}

void Level::AddMob(Mob* mob)
{
	m_Mobs.push_back(mob);
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

void Level::AddItem(Item* item)
{
	m_Items.push_back(item);
}

void Level::RemoveItem(Item* item)
{
	for (size_t i = 0; i < m_Items.size(); i++)
	{
		if (m_Items[i] == item)
		{
			delete m_Items[i];
			m_Items[i] = nullptr;
			return;
		}
	}
}

void Level::AddItemToBeRemoved(Item* item)
{
	for (size_t i = 0; i < m_ItemsToBeRemoved.size(); ++i)
	{
		if (m_ItemsToBeRemoved[i] == item) return;
	}
	m_ItemsToBeRemoved.push_back(item);

	for (size_t i = 0; i < m_Items.size(); ++i)
	{
		if (m_Items[i] == item)
		{
			m_Items[i] = nullptr;
			break;
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

bool Level::OnKeyPress(sf::Event::KeyEvent keyEvent, bool keyPressed)
{
	if (keyPressed)
	{
		switch (keyEvent.code)
		{
		case sf::Keyboard::E:
		{
			if (!m_CurrentSpeech.empty() &&
				m_SpeechLetterTransition.GetPercentComplete() < 1.0f)
			{
				m_SpeechLetterTransition.SetFinished();
			}
			else
			{
				InteractWithHighlightedItem();
			}
		} break;
		}
	}
	return true;
}

void Level::OnKeyRelease(sf::Event::KeyEvent keyEvent)
{
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
	APEX->SetCursor(ApexCursor::NORMAL);
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
