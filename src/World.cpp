
#include "World.h"
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
#include "ApexNPC.h"
#include "ApexMath.h"
#include "Coin.h"

#include <fstream>

using namespace nlohmann;

const sf::Uint32 World::MILLISECONDS_PER_SPEECH_BUBBLE_LETTER = 50;

World::World(int worldIndex) :
	m_WorldIndex(worldIndex)
{
	m_DebugOverlay = new ApexDebug();
	m_ShowingDebugOverlay = false;

	m_BulletManager = new BulletManager();

	m_Map = new Map(this, "resources/worlds/" + std::to_string(m_WorldIndex) + "/");

	m_Width = m_Map->GetTilesWide() * m_Map->GetTileSize();
	m_Height = m_Map->GetTilesHigh() * m_Map->GetTileSize();

	ReadBuildingData();

	m_Player = new Player(this, m_Map);
	m_Player->GetPhysicsActor()->SetPosition(m_Map->GetPlayerSpawnPosition());

	const sf::Vector2u windowSize = APEX->GetWindowSize();
	const float aspectRatio = float(windowSize.x / windowSize.y);
	m_Camera = new Camera(sf::Vector2f(float(windowSize.x), float(windowSize.y)));
	m_Camera->SetZoom(2.0f);
	m_PauseScreen = new ApexPauseScreen(this);

	const float minimapWidth = 200.0f;
	m_Minimap = new Minimap(this, sf::Vector2f(minimapWidth, minimapWidth / aspectRatio));

	m_HUD = new HUD(this);

	m_SpeechBubbleSpriteSheet = ApexSpriteSheet(TextureManager::GetTexture(TextureManager::SPEECH_BUBBLE), 16, 16);

	m_SpeechLetterTransition.SetEaseType(ApexTransition::EaseType::LINEAR);
}

World::~World()
{
	delete m_Map;

	for (size_t i = 0; i < m_Maps.size(); ++i)
	{
		delete m_Maps[i];
	}
	m_Maps.clear();

	delete m_Player;
	delete m_Camera;
	delete m_Minimap;
	delete m_HUD;

	delete m_DebugOverlay;
	delete m_BulletManager;
	delete m_PauseScreen;
}

void World::Reset()
{
	m_Player->Reset();
	m_BulletManager->Reset();

	m_Map->Reset();
	m_Map->CreatePhysicsActors(this);

	for (size_t i = 0; i < m_Maps.size(); i++)
	{
		m_Maps[i]->Reset();
	}

	ClearSpeechShowing();
}

void World::ReadBuildingData()
{
	std::ifstream fileInStream;

	std::string directoryPath = "resources/worlds/" + std::to_string(m_WorldIndex) + "/buildings/";
	const std::string fileName = "tiles.json";

	int buildingIndex = 0;
	bool fileFound;
	do
	{
		fileFound = false;

		const std::string fullPath = directoryPath + std::to_string(buildingIndex) + "/" + fileName;
		fileInStream.open(fullPath);

		if (fileInStream)
		{
			fileFound = true;

			Map* map = new Map(this, directoryPath + std::to_string(buildingIndex) + "/");
			m_Maps.push_back(map);

			++buildingIndex;
		}
	} while (fileFound);
	assert(buildingIndex == m_Maps.size());
}

void World::CreateMapPhysicsActors(int mapIndex)
{
	m_Maps[mapIndex]->CreatePhysicsActors(this);
}

void World::DeleteMapPhysicsActors(int mapIndex)
{
	m_Maps[mapIndex]->DestroyPhysicsActors();
}

bool World::IsPaused() const
{
	return m_Paused;
}

void World::TogglePaused(bool pauseSounds)
{
	SetPaused(!m_Paused, pauseSounds);
}

Map* World::GetCurrentMap()
{
	if (m_CurrentMapShowingIndex == -1) return m_Map;
	else return m_Maps[m_CurrentMapShowingIndex];
}

void World::SetPaused(bool paused, bool pauseSounds)
{
	m_Paused = paused;
	APEX->SetPhysicsPaused(m_Paused);
	m_DebugOverlay->ClearAllInput();

	if (m_Paused)
	{
		m_PauseScreen->SetScreenShowing(ApexPauseScreen::Screen::MAIN);
	}

	if (pauseSounds) ApexAudio::SetAllPaused(m_Paused);
}

void World::Tick(sf::Time elapsed)
{
	if (m_Paused)
	{
		m_PauseScreen->Tick(elapsed);
		return;
	}

	if (APEX->IsFadingOut() && m_FadingOutTo != FadingOutTo::NONE)
	{
		switch (m_FadingOutTo)
		{
		case FadingOutTo::ENTER_BUILDING:
			m_Map->DestroyPhysicsActors();
			CreateMapPhysicsActors(m_MapToTravelToIndex);
			m_Player->GetPhysicsActor()->SetPosition(m_Maps[m_MapToTravelToIndex]->GetPlayerSpawnPosition());
			m_CurrentMapShowingIndex = m_MapToTravelToIndex;
			m_MapToTravelToIndex = -1;
			break;
		case FadingOutTo::EXIT_BUILDING:
			DeleteMapPhysicsActors(m_CurrentMapShowingIndex);
			m_Map->CreatePhysicsActors(this);
			m_Player->GetPhysicsActor()->SetPosition(m_Map->GetPlayerSpawnPosition());
			m_CurrentMapShowingIndex = -1;
			break;
		case FadingOutTo::NONE:
		default:
			break;
		}

		m_FadingOutTo = FadingOutTo::NONE;
		return;
	}


	GetCurrentMap()->Tick(elapsed);

	//m_Minimap->Tick(elapsed);
	m_Player->Tick(elapsed);
	m_Camera->Tick(elapsed, this);
	m_BulletManager->Tick(elapsed);
	m_HUD->Tick(elapsed);

	m_ParticleManager.Tick(elapsed);
	if (m_ShowingDebugOverlay) m_DebugOverlay->Tick(elapsed);

	m_SpeechLetterTransition.Tick(elapsed);
	if (!m_CurrentSpeech.empty() &&
		m_SpeechLetterTransition.GetPercentComplete() < 1.0f)
	{
		if (!ApexAudio::IsSoundEffectPlaying(ApexAudio::Sound::TYPING_1) &&
			!ApexAudio::IsSoundEffectPlaying(ApexAudio::Sound::TYPING_2) &&
			!ApexAudio::IsSoundEffectPlaying(ApexAudio::Sound::TYPING_3))
		{
			int rand = std::rand() % 3;
			ApexAudio::PlaySoundEffect(rand == 0 ? ApexAudio::Sound::TYPING_1 :
				rand == 1 ? ApexAudio::Sound::TYPING_2 : ApexAudio::Sound::TYPING_3);
		}
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

void World::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	sf::View cameraView = m_Camera->GetCurrentView();
	target.setView(cameraView);

	GetCurrentMap()->Draw(target, states);

	m_BulletManager->Draw(target, states);
	m_Player->Draw(target, states);

	m_ParticleManager.Draw(target, states);

	// Static elements
	target.setView(target.getDefaultView());

	//m_Minimap->Draw(target, states);
	m_HUD->Draw(target, states);

	if (!m_CurrentSpeech.empty())
	{
		const unsigned int SPEECH_FONT_SIZE = 48;
		const sf::Color SPEECH_FONT_COLOR = sf::Color(25, 25, 10);
		const sf::Color SPEECH_SHADOW_COLOR = sf::Color(204, 197, 173);
		std::string speechString = m_CurrentSpeech.substr(0, m_SpeechLetterTransition.GetCurrentTransitionData() + 1);
		sf::Text speechText(m_CurrentSpeech, APEX->FontPixelFJ8, SPEECH_FONT_SIZE);

		const sf::FloatRect bounds = speechText.getLocalBounds();
		speechText.setString(speechString);

		// Draw the speech bubble as nine differently scaled parts of a rectangle
		const sf::Vector2f defaultScale(5.0f, 5.0f);
		const float frameWidth = m_SpeechBubbleSpriteSheet.GetFrameWidth() * defaultScale.x;
		const float frameHeight = m_SpeechBubbleSpriteSheet.GetFrameHeight() * defaultScale.y;
		const sf::Vector2f speechBubbleSize(bounds.width + frameWidth * 2, bounds.height + frameHeight * 2);
		assert(speechBubbleSize.x >= frameWidth * 2);
		assert(speechBubbleSize.y >= frameHeight * 2);
		const sf::Vector2f interiorSize(speechBubbleSize - sf::Vector2f(frameWidth * 2, frameHeight * 2));
		const sf::Vector2f interiorScale(interiorSize.x / frameWidth * defaultScale.x, interiorSize.y / frameHeight * defaultScale.y);
		const sf::Vector2f windowSize = static_cast<sf::Vector2f>(APEX->GetWindowSize());
		const sf::Vector2f speechTopLeft(windowSize.x / 2.0f - speechBubbleSize.x / 2.0f, windowSize.y - speechBubbleSize.y * 1.1f);

		// Draw all four corners (since they are never scaled)
		states.transform.translate(speechTopLeft);
		m_SpeechBubbleSpriteSheet.SetSpriteScale(defaultScale);
		m_SpeechBubbleSpriteSheet.Draw(target, states, 0, 0); // Top left

		states.transform.translate(speechBubbleSize.x - frameWidth, 0);
		m_SpeechBubbleSpriteSheet.Draw(target, states, 2, 0); // Top right

		states.transform.translate(0, speechBubbleSize.y - frameHeight);
		m_SpeechBubbleSpriteSheet.Draw(target, states, 2, 2); // Bottom right

		states.transform.translate(-(speechBubbleSize.x - frameWidth), 0);
		m_SpeechBubbleSpriteSheet.Draw(target, states, 0, 2); // Bottom left

		states.transform.translate(frameWidth, -(speechBubbleSize.y - frameHeight));
		m_SpeechBubbleSpriteSheet.SetSpriteScale(interiorScale.x, defaultScale.y);
		m_SpeechBubbleSpriteSheet.Draw(target, states, 1, 0); // Top middle

		states.transform.translate(0, speechBubbleSize.y - frameHeight);
		m_SpeechBubbleSpriteSheet.Draw(target, states, 1, 2); // Bottom middle

		states.transform.translate(-frameWidth, -(speechBubbleSize.y - frameHeight) + frameHeight);
		m_SpeechBubbleSpriteSheet.SetSpriteScale(defaultScale.x, interiorScale.y);
		m_SpeechBubbleSpriteSheet.Draw(target, states, 0, 1); // Left middle

		states.transform.translate(speechBubbleSize.x - frameWidth, 0);
		m_SpeechBubbleSpriteSheet.Draw(target, states, 2, 1); // Right middle

		states.transform.translate(-(speechBubbleSize.x - frameWidth) + frameWidth, 0);
		m_SpeechBubbleSpriteSheet.SetSpriteScale(interiorScale);
		m_SpeechBubbleSpriteSheet.Draw(target, states, 1, 1); // Middle middle

		speechText.setColor(SPEECH_SHADOW_COLOR);
		states.transform.translate(defaultScale);
		target.draw(speechText, states);

		speechText.setColor(SPEECH_FONT_COLOR);
		states.transform.translate(-defaultScale);
		target.draw(speechText, states);
	}

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

bool World::IsShowingSpeechBubble() const
{
	return !m_CurrentSpeech.empty();
}

void World::SetCurrentSpeechShowing(const std::string& speech)
{
	m_CurrentSpeech = speech;

	const size_t stringLength = speech.length();
	m_SpeechLetterTransition.Create(0, stringLength, sf::milliseconds(MILLISECONDS_PER_SPEECH_BUBBLE_LETTER * stringLength));
	m_SpeechLetterTransition.Restart();

	m_Player->StopMoving();
}

void World::ClearSpeechShowing()
{
	m_CurrentSpeech = "";
}

void World::OnUnmappedKeypress(sf::Event::KeyEvent event)
{
	assert(m_Paused && m_PauseScreen != nullptr);

	m_PauseScreen->OnUnmappedKeyPress(event);
}

void World::EnterMap(int buildingIndex)
{
	APEX->StartFadeInOut();
	m_FadingOutTo = FadingOutTo::ENTER_BUILDING;
	m_MapToTravelToIndex = buildingIndex;
}

void World::ExitMap()
{
	APEX->StartFadeInOut();
	m_FadingOutTo = FadingOutTo::EXIT_BUILDING;
}

int World::GetWorldIndex() const
{
	return m_WorldIndex;
}

void World::AddParticle(ApexParticle* spriteSheet)
{
	m_ParticleManager.AddParticle(spriteSheet);
}

void World::BeginContact(PhysicsActor* thisActor, PhysicsActor* otherActor)
{
}

void World::EndContact(PhysicsActor* thisActor, PhysicsActor* otherActor)
{
}

void World::PreSolve(PhysicsActor* thisActor, PhysicsActor* otherActor, bool & enableContact)
{
}

bool World::OnKeyPress(ApexKeyboard::Key key, bool keyPressed)
{
	if (keyPressed)
	{
		switch (key)
		{
		case ApexKeyboard::PAUSE:
		{
			if (!m_Paused) // The pause screen will unpause itself
			{
				SetPaused(true, true);
				return true;
			}
		} break;
		case ApexKeyboard::INTERACT:
		{
			if (!m_CurrentSpeech.empty() &&
				m_SpeechLetterTransition.GetPercentComplete() < 1.0f)
			{
				m_SpeechLetterTransition.SetFinished();
			}
			else
			{
				GetCurrentMap()->InteractWithHighlightedItem();
			}
		} break;
		}
	}
	return false;
}

void World::OnKeyRelease(ApexKeyboard::Key key)
{
}

unsigned int World::GetWidth() const
{
	return m_Width;
}

unsigned int World::GetHeight() const
{
	return m_Height;
}

Player* World::GetPlayer()
{
	return m_Player;
}

BulletManager* World::GetBulletManager()
{
	return m_BulletManager;
}

void World::ToggleDebugOverlay()
{
	m_ShowingDebugOverlay = !m_ShowingDebugOverlay;
	m_DebugOverlay->ClearAllInput();
	APEX->SetCursor(ApexCursor::NORMAL);
}

bool World::IsShowingDebugOverlay() const
{
	return m_ShowingDebugOverlay;
}

sf::View World::GetCurrentView() const
{
	return m_Camera->GetCurrentView();
}

void World::JoltCamera(float xAmount, float yAmount)
{
	m_Camera->Jolt(xAmount, yAmount);
}

void World::SetScreenShake(float xScale, float yScale)
{
	m_Camera->Shake(xScale, yScale);
}