
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

const sf::Uint32 World::MILLISECONDS_PER_SPEECH_BUBBLE_LETTER = 40;
const sf::Uint32 World::MILLISECONDS_OF_SPEECH_BUBBLE_ANIMATION = 350;

const float World::SPEECH_BUBBLE_HIDE_SCALE = 0.75f;
const float World::SPEECH_BUBBLE_HIDE_Y_OFFSET = 250.0f;
const ApexTransition::EaseType World::SPEECH_BUBBLE_EASE_TYPE = ApexTransition::EaseType::EXPONENTIAL_IN_OUT;

World::World(int worldIndex) :
	m_WorldIndex(worldIndex)
{
	m_DebugOverlay = new ApexDebug();
	m_ShowingDebugOverlay = false;

	m_BulletManager = new BulletManager();

	m_Map = new Map(this, -1, "resources/worlds/" + std::to_string(m_WorldIndex) + "/");

	ReadBuildingData();

	m_Player = new Player(this, m_Map);

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
	for (size_t i = 0; i < m_Maps.size(); i++)
	{
		m_Maps[i]->Reset();
	}

	m_Width = m_Map->GetTilesWide() * m_Map->GetTileSize();
	m_Height = m_Map->GetTilesHigh() * m_Map->GetTileSize();

	GetCurrentMap()->CreatePhysicsActors(this);
	m_Player->CreatePhysicsActor();
	m_Player->GetPhysicsActor()->SetPosition(GetCurrentMap()->GetPlayerSpawnPosition());

	m_IsHidingSpeechBubble = false;
	m_CurrentSpeech.speechText.setString("");
	m_CurrentSpeech.speechString.clear();
	m_CurrentSpeech.interiorSize.SetFinished();
	m_SpeechLetterTransition.SetFinished();
	m_SpeechBubbleTransition.SetFinished();
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

			Map* map = new Map(this, buildingIndex, directoryPath + std::to_string(buildingIndex) + "/");
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

	m_Player->Tick(elapsed);
	m_Camera->Tick(elapsed, this);
	m_BulletManager->Tick(elapsed);
	m_HUD->Tick(elapsed);

	m_ParticleManager.Tick(elapsed);
	if (m_ShowingDebugOverlay) m_DebugOverlay->Tick(elapsed);

	m_CurrentSpeech.interiorSize.Tick(elapsed);
	if (!m_CurrentSpeech.interiorSize.IsFinished())
	{
		CalculateSpeechBubbleInteriorSize();
	}
	m_SpeechBubbleTransition.Tick(elapsed);
	if (m_IsHidingSpeechBubble)
	{
		if (m_SpeechBubbleTransition.IsFinished())
		{
			m_CurrentSpeech.speechString.clear();
			m_CurrentSpeech.speechText.setString("");
			sf::Vector2f start;
			sf::Vector2f end;
			m_CurrentSpeech.interiorSize.Create(start, end, sf::milliseconds(MILLISECONDS_OF_SPEECH_BUBBLE_ANIMATION));
			m_CurrentSpeech.interiorSize.SetFinished();
			m_IsHidingSpeechBubble = false;
		}
	}
	if (m_SpeechBubbleTransition.IsFinished())
	{
		m_SpeechLetterTransition.Tick(elapsed);
		if (IsShowingSpeechBubble() &&
			m_SpeechLetterTransition.GetPercentComplete() < 1.0f)
		{
			if (!ApexAudio::IsSoundEffectPlaying(ApexAudio::Sound::TYPING_1) &&
				!ApexAudio::IsSoundEffectPlaying(ApexAudio::Sound::TYPING_2) &&
				!ApexAudio::IsSoundEffectPlaying(ApexAudio::Sound::TYPING_3))
			{
				const char currentSoundPronouncing = m_CurrentSpeech.speechString[m_SpeechLetterTransition.GetCurrentTransitionData()];
				if (currentSoundPronouncing >= 'a' && currentSoundPronouncing < 'j')
				{
					ApexAudio::PlaySoundEffect(ApexAudio::Sound::TYPING_1);
				}
				else if (currentSoundPronouncing >= 'j' && currentSoundPronouncing < 'r')
				{
					ApexAudio::PlaySoundEffect(ApexAudio::Sound::TYPING_2);
				}
				else
				{
					ApexAudio::PlaySoundEffect(ApexAudio::Sound::TYPING_3);
				}
			}
		}
	}
}

void World::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	sf::View cameraView = m_Camera->GetCurrentView();
	target.setView(cameraView);

	GetCurrentMap()->Draw(target, states);
	m_BulletManager->Draw(target, states);
	m_ParticleManager.Draw(target, states);

	// Static elements
	target.setView(target.getDefaultView());

	m_HUD->Draw(target, states);

	if (IsShowingSpeechBubble())
	{
		DrawSpeechBubble(target, states);
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

void World::DrawSpeechBubble(sf::RenderTarget& target, sf::RenderStates states)
{
	const sf::Color SPEECH_FONT_COLOR = sf::Color(25, 25, 10);
	const sf::Color SPEECH_SHADOW_COLOR = sf::Color(204, 197, 173);

	const sf::Vector2f windowSize = static_cast<sf::Vector2f>(APEX->GetWindowSize());

	std::string speechString = m_CurrentSpeech.speechString.substr(0, m_SpeechLetterTransition.GetCurrentTransitionData() + 1);
	m_CurrentSpeech.speechText.setString(speechString);

	const sf::Vector2f interiorSize = m_CurrentSpeech.interiorSize.GetCurrentTransitionData();
	m_CurrentSpeech.interiorScale = sf::Vector2f(
		interiorSize.x / m_CurrentSpeech.frameWidth * m_CurrentSpeech.scale.x,
		interiorSize.y / m_CurrentSpeech.frameHeight * m_CurrentSpeech.scale.y);

	const sf::Transformable speechBubbleTransformable = m_SpeechBubbleTransition.GetCurrentTransformable();
	states.transform.scale(speechBubbleTransformable.getScale(), 
		sf::Vector2f(windowSize.x / 2.0f, windowSize.y));
	states.transform.translate(speechBubbleTransformable.getPosition());

	const float innerWidth = interiorSize.x;
	const float innerHeight = interiorSize.y;

	// Draw the speech bubble as nine differently scaled parts of a rectangle
	// Draw all four corners (since they are never scaled)
	m_SpeechBubbleSpriteSheet.SetSpriteScale(m_CurrentSpeech.scale);
	states.transform.translate(m_CurrentSpeech.speechTopLeft);
	const sf::Transform topLeftTransform = states.transform;
	m_SpeechBubbleSpriteSheet.Draw(target, states, 0, 0); // Top left
	
	states.transform.translate(innerWidth + m_CurrentSpeech.frameWidth, 0);
	m_SpeechBubbleSpriteSheet.Draw(target, states, 2, 0); // Top right
	
	states.transform.translate(0, innerHeight + m_CurrentSpeech.frameHeight);
	m_SpeechBubbleSpriteSheet.Draw(target, states, 2, 2); // Bottom right
	
	states.transform.translate(-(innerWidth + m_CurrentSpeech.frameWidth), 0);
	m_SpeechBubbleSpriteSheet.Draw(target, states, 0, 2); // Bottom left

	// Stetch all other sections accordingly
	states.transform = topLeftTransform;
	states.transform.translate(m_CurrentSpeech.frameWidth, 0);
	m_SpeechBubbleSpriteSheet.SetSpriteScale(m_CurrentSpeech.interiorScale.x, m_CurrentSpeech.scale.y);
	m_SpeechBubbleSpriteSheet.Draw(target, states, 1, 0); // Top middle

	states.transform.translate(0, innerHeight + m_CurrentSpeech.frameHeight);
	m_SpeechBubbleSpriteSheet.Draw(target, states, 1, 2); // Bottom middle

	states.transform.translate(-m_CurrentSpeech.frameWidth, -innerHeight);
	m_SpeechBubbleSpriteSheet.SetSpriteScale(m_CurrentSpeech.scale.x, m_CurrentSpeech.interiorScale.y);
	m_SpeechBubbleSpriteSheet.Draw(target, states, 0, 1); // Left middle

	states.transform.translate(innerWidth + m_CurrentSpeech.frameWidth, 0);
	m_SpeechBubbleSpriteSheet.Draw(target, states, 2, 1); // Right middle

	states.transform.translate(-innerWidth, 0);
	m_SpeechBubbleSpriteSheet.SetSpriteScale(m_CurrentSpeech.interiorScale);
	m_SpeechBubbleSpriteSheet.Draw(target, states, 1, 1); // Middle middle

	if (m_SpeechBubbleTransition.IsFinished())
	{
		states.transform = topLeftTransform;
		states.transform.translate(m_CurrentSpeech.frameWidth, m_CurrentSpeech.frameHeight);
		
		m_CurrentSpeech.speechText.setColor(SPEECH_SHADOW_COLOR);
		states.transform.translate(m_CurrentSpeech.scale);
		target.draw(m_CurrentSpeech.speechText, states); // Text shadow

		m_CurrentSpeech.speechText.setColor(SPEECH_FONT_COLOR);
		states.transform.translate(-m_CurrentSpeech.scale);
		target.draw(m_CurrentSpeech.speechText, states); // Text
	}
}

void World::SetCurrentSpeechShowing(const std::string& speech)
{
	assert(!speech.empty());
	const bool wasShowingSpeech = IsShowingSpeechBubble();
	const sf::Vector2f previousInteriorSize = m_CurrentSpeech.interiorSize.GetCurrentTransitionData();

	const unsigned int SPEECH_FONT_SIZE = 48;

	m_CurrentSpeech.speechString = speech;
	m_CurrentSpeech.speechText = sf::Text(speech, APEX->FontPixelFJ8, SPEECH_FONT_SIZE);
	m_CurrentSpeech.bounds = m_CurrentSpeech.speechText.getLocalBounds();
	m_CurrentSpeech.speechText.setString("");
	m_CurrentSpeech.scale = sf::Vector2f(5.0f, 5.0f);

	m_CurrentSpeech.frameWidth = m_SpeechBubbleSpriteSheet.GetFrameWidth() * m_CurrentSpeech.scale.x;
	m_CurrentSpeech.frameHeight = m_SpeechBubbleSpriteSheet.GetFrameHeight() * m_CurrentSpeech.scale.y;

	m_CurrentSpeech.speechBubbleSize = sf::Vector2f(
		m_CurrentSpeech.bounds.width + m_CurrentSpeech.frameWidth * 2,
		m_CurrentSpeech.bounds.height + m_CurrentSpeech.frameHeight * 2);

	sf::Vector2f interiorStartSize = previousInteriorSize;
	sf::Vector2f interiorEndSize = sf::Vector2f(m_CurrentSpeech.bounds.width, m_CurrentSpeech.bounds.height);
	m_CurrentSpeech.interiorSize.Create(interiorStartSize, interiorEndSize,
		sf::milliseconds(MILLISECONDS_OF_SPEECH_BUBBLE_ANIMATION), SPEECH_BUBBLE_EASE_TYPE);
	if (previousInteriorSize.x == 0 || previousInteriorSize.y == 0)
	{
		m_CurrentSpeech.interiorSize.SetFinished();
	}
	CalculateSpeechBubbleInteriorSize();

	const size_t stringLength = speech.length();
	m_SpeechLetterTransition.Create(0, stringLength, sf::milliseconds(MILLISECONDS_PER_SPEECH_BUBBLE_LETTER * stringLength));
	m_SpeechLetterTransition.Restart();

	sf::Transformable speechBubbleTransformableStart;
	sf::Transformable speechBubbleTransformableEnd;
	if (!wasShowingSpeech)
	{
		speechBubbleTransformableStart.setScale(SPEECH_BUBBLE_HIDE_SCALE, SPEECH_BUBBLE_HIDE_SCALE);
		speechBubbleTransformableStart.setPosition(0, SPEECH_BUBBLE_HIDE_Y_OFFSET);
	}

	m_SpeechBubbleTransition.Create(speechBubbleTransformableStart, speechBubbleTransformableEnd, 
		sf::milliseconds(MILLISECONDS_OF_SPEECH_BUBBLE_ANIMATION), SPEECH_BUBBLE_EASE_TYPE);
	m_SpeechBubbleTransition.Restart();

	m_Player->StopMoving();
}

void World::CalculateSpeechBubbleInteriorSize()
{
	const sf::Vector2f windowSize = static_cast<sf::Vector2f>(APEX->GetWindowSize());

	const sf::Vector2f interiorSize = m_CurrentSpeech.interiorSize.GetCurrentTransitionData();
	m_CurrentSpeech.interiorScale = sf::Vector2f(
		interiorSize.x / m_CurrentSpeech.frameWidth * m_CurrentSpeech.scale.x,
		interiorSize.y / m_CurrentSpeech.frameHeight * m_CurrentSpeech.scale.y);
	m_CurrentSpeech.speechTopLeft = sf::Vector2f(
		windowSize.x / 2.0f - (interiorSize.x + m_CurrentSpeech.frameWidth * 2) / 2.0f,
		windowSize.y - (interiorSize.y + m_CurrentSpeech.frameHeight * 2) * 1.1f);
}

bool World::IsShowingSpeechBubble() const
{
	return !m_CurrentSpeech.speechString.empty();
}

void World::ClearSpeechShowing()
{
	sf::Transformable start;
	sf::Transformable end;
	start.setScale(SPEECH_BUBBLE_HIDE_SCALE, SPEECH_BUBBLE_HIDE_SCALE);
	start.setPosition(0, SPEECH_BUBBLE_HIDE_Y_OFFSET);
	m_SpeechBubbleTransition.Create(start, end, sf::milliseconds(MILLISECONDS_OF_SPEECH_BUBBLE_ANIMATION), SPEECH_BUBBLE_EASE_TYPE);
	m_SpeechBubbleTransition.SwapAndRestart();
	m_IsHidingSpeechBubble = true;
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
			if (IsShowingSpeechBubble())
			{
				bool skipped = false;
				if (!m_CurrentSpeech.interiorSize.IsFinished())
				{
					m_CurrentSpeech.interiorSize.SetFinished();
					skipped = true;
				}
				if (!m_SpeechBubbleTransition.IsFinished())
				{
					m_SpeechBubbleTransition.SetFinished();
					skipped = true;
				}
				if (!m_SpeechLetterTransition.IsFinished())
				{
					m_SpeechLetterTransition.SetFinished();
					skipped = true;
				}

				if (!skipped)
				{
					GetCurrentMap()->InteractWithHighlightedItem();
				}
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