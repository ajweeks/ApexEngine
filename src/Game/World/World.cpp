
#include "Game\World\World.h"
#include "Game\Game.h"
#include "Game\PauseScreen.h"
#include "Game\HUD.h"
#include "Game\PauseScreen.h"
#include "Game\World\Map.h"
#include "Game\World\Mob.h"
#include "Game\World\Sheep.h"
#include "Game\World\Entity.h"
#include "Game\World\Item.h"
#include "Game\World\AmmoDrop.h"
#include "Game\World\AICharacter.h"
#include "Game\World\Coin.h"
#include "Game\World\Minimap.h"
#include "Game\World\Camera.h"

#include "Apex\ApexMain.h"
#include "Apex\Physics\PhysicsActor.h"
#include "Apex\IO\Audio.h"
#include "Apex\ApexMath.h"
#include "Apex\IO\Keyboard.h"

#include <fstream>

using namespace nlohmann;

const sf::Uint32 World::MILLISECONDS_PER_SPEECH_BUBBLE_LETTER = 40;
const sf::Uint32 World::MILLISECONDS_OF_SPEECH_BUBBLE_ANIMATION = 350;

const float World::SPEECH_BUBBLE_HIDE_SCALE = 0.75f;
const float World::SPEECH_BUBBLE_HIDE_Y_OFFSET = 250.0f;
const apex::Transition::EaseType World::SPEECH_BUBBLE_EASE_TYPE = apex::Transition::EaseType::EXPONENTIAL_IN_OUT;

World::World(int worldIndex, apex::StateManager& stateManager) :
	m_WorldIndex(worldIndex),
	m_StateManager(stateManager)
{
	m_BulletManager = new BulletManager();

	CreateMaps();

	m_Player = new Player(*this, *m_Map);

	const sf::Vector2u windowSize = APEX->GetWindowSize();
	const float aspectRatio = float(windowSize.x / windowSize.y);
	m_Camera = new Camera(sf::Vector2f(float(windowSize.x), float(windowSize.y)), *this);
	m_Camera->SetZoom(2.0f);
	m_PauseScreen = new PauseScreen(*this, m_StateManager);

	const float minimapWidth = 200.0f;
	m_Minimap = new Minimap(*this, sf::Vector2f(minimapWidth, minimapWidth / aspectRatio));

	m_HUD = new HUD(*this);

	m_SpeechBubbleSpriteSheet = apex::SpriteSheet(apex::TextureManager::GetTexture(Texture::SPEECH_BUBBLE), 16, 16);
	m_SpeechLetterTransition.SetEaseType(apex::Transition::EaseType::LINEAR);
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

	GetCurrentMap()->CreatePhysicsActors(this);
	m_Player->CreatePhysicsActor();
	m_Player->GetPhysicsActor()->SetPosition(GetCurrentMap()->GetPlayerSpawnPosition());

	m_IsHidingSpeechBubble = false;
	m_CurrentSpeech.speechText.setString("");
	m_CurrentSpeech.speechString.clear();
	m_CurrentSpeech.interiorSize.SetFinished();
	m_SpeechLetterTransition.SetFinished();
	m_SpeechBubbleTransition.SetFinished();

	m_FadingOutTo = FadingOutTo::NONE;
}

void World::CreateMaps()
{
	std::ifstream fileInStream;

	std::string directoryPath = "resources/worlds/" + std::to_string(m_WorldIndex) + "/";
	const std::string fileName = "tiles.json";

	m_Map = new Map(*this, -1, directoryPath);

	int buildingIndex = 0;
	bool fileFound;
	do
	{
		fileFound = false;

		const std::string subDirectory = directoryPath + "buildings/" + std::to_string(buildingIndex) + "/";
		const std::string fullPath = subDirectory + fileName;
		fileInStream.open(fullPath);

		if (fileInStream)
		{
			fileFound = true;

			Map* map = new Map(*this, buildingIndex, subDirectory);
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

void World::ToggleLightingEnabled()
{
	SetLightingEnabled(!m_LightingEnabled);
}

void World::SetLightingEnabled(bool enabled)
{
	m_LightingEnabled = enabled;
}

bool World::IsLightingEnabled() const
{
	return m_LightingEnabled;
}

void World::SetPaused(bool paused, bool pauseSounds)
{
	m_Paused = paused;
	APEX->SetPhysicsPaused(m_Paused);

	if (m_Paused)
	{
		m_PauseScreen->SetScreenShowing(PauseScreen::Screen::MAIN);
	}
	else
	{
		m_PauseScreen->SetScreenShowing(PauseScreen::Screen::NONE);
	}

	if (pauseSounds) apex::Audio::SetAllPaused(m_Paused);
}

void World::Tick(sf::Time elapsed)
{
	if (m_Paused)
	{
		m_PauseScreen->Tick(elapsed);
		return;
	}

	if (m_FadingOutTo != FadingOutTo::NONE)
	{
		if (!APEX->IsFadingIn())
		{
			switch (m_FadingOutTo)
			{
			case FadingOutTo::ENTER_BUILDING:
			{
				m_Map->DestroyPhysicsActors();
				CreateMapPhysicsActors(m_MapToTravelToIndex);
				m_Player->GetPhysicsActor()->SetPosition(m_Maps[m_MapToTravelToIndex]->GetPlayerSpawnPosition());
				m_CurrentMapShowingIndex = m_MapToTravelToIndex;
				m_MapToTravelToIndex = -1;
				m_FadingOutTo = FadingOutTo::NONE;
				m_Camera->SnapToPlayer();
				break;
			}
			case FadingOutTo::EXIT_BUILDING:
			{
				DeleteMapPhysicsActors(m_CurrentMapShowingIndex);
				m_Map->CreatePhysicsActors(this);
				m_Player->GetPhysicsActor()->SetPosition(m_Map->GetPlayerSpawnPosition());
				m_CurrentMapShowingIndex = -1;
				m_FadingOutTo = FadingOutTo::NONE;
				m_Camera->SnapToPlayer();
				break;
			}
			case FadingOutTo::NONE:
			default:
			{
			} break;
			}
		}

		return;
	}

	GetCurrentMap()->Tick(elapsed);

	m_Player->Tick(elapsed);
	m_Camera->Tick(elapsed);
	m_BulletManager->Tick(elapsed);
	m_HUD->Tick(elapsed);

	m_ParticleManager.Tick(elapsed);

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
			if (!apex::Audio::IsSoundEffectPlaying(size_t(Sound::TYPING_1)) &&
				!apex::Audio::IsSoundEffectPlaying(size_t(Sound::TYPING_2)) &&
				!apex::Audio::IsSoundEffectPlaying(size_t(Sound::TYPING_3)))
			{
				const char currentSound = m_CurrentSpeech.speechString[m_SpeechLetterTransition.GetCurrentTransitionData()];
				if (currentSound >= 'a' && currentSound < 'j') apex::Audio::PlaySoundEffect(size_t(Sound::TYPING_1));
				else if (currentSound >= 'j' && currentSound < 'r') apex::Audio::PlaySoundEffect(size_t(Sound::TYPING_2));
				else apex::Audio::PlaySoundEffect(size_t(Sound::TYPING_3));
			}
		}
	}
}

void World::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	sf::View cameraView = m_Camera->GetCurrentView();
	target.setView(cameraView);

	GetCurrentMap()->Draw(target, states, m_LightingEnabled);
	m_BulletManager->Draw(target, states);
	m_ParticleManager.Draw(target, states);

	// Static elements
	target.setView(target.getDefaultView());

	m_HUD->Draw(target, states);

	if (IsShowingSpeechBubble())
	{
		DrawSpeechBubble(target, states);
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
		
		m_CurrentSpeech.speechText.setFillColor(SPEECH_SHADOW_COLOR);
		states.transform.translate(m_CurrentSpeech.scale);
		target.draw(m_CurrentSpeech.speechText, states); // Text shadow

		m_CurrentSpeech.speechText.setFillColor(SPEECH_FONT_COLOR);
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
	m_CurrentSpeech.speechText = sf::Text(speech, Game::FontPixelFJ8, SPEECH_FONT_SIZE);
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
	if (m_FadingOutTo == FadingOutTo::NONE)
	{
		APEX->StartFadeInOut();
		m_FadingOutTo = FadingOutTo::ENTER_BUILDING;
		m_MapToTravelToIndex = buildingIndex;
		m_Player->StopMoving();
	}
}

void World::ExitMap()
{
	if (m_FadingOutTo == FadingOutTo::NONE)
	{
		APEX->StartFadeInOut();
		m_FadingOutTo = FadingOutTo::EXIT_BUILDING;
		m_Player->StopMoving();
	}
}

int World::GetWorldIndex() const
{
	return m_WorldIndex;
}

void World::AddParticle(apex::Particle* spriteSheet)
{
	m_ParticleManager.AddParticle(spriteSheet);
}

void World::BeginContact(apex::Contact* contact)
{
}

void World::EndContact(apex::Contact* contact)
{
}

void World::PreSolve(apex::Contact* contact, bool & enableContact)
{
}

bool World::OnKeyPress(apex::Keyboard::Key key, bool keyPressed)
{
	if (APEX->IsFadingIn() || APEX->IsFadingOut()) return false;

	if (keyPressed)
	{
		switch (Key(key.vkCode))
		{
		case Key::PAUSE:
		{
			if (!m_Paused) // The pause screen will unpause itself
			{
				SetPaused(true, true);
				return true;
			}
		} break;
		case Key::INTERACT:
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
				return true;
			}
			else
			{
				if (GetCurrentMap()->HasHighlightedItem())
				{
					GetCurrentMap()->InteractWithHighlightedItem();
					return true;
				}
			}
		} break;
		}
	}
	return false;
}

void World::OnKeyRelease(apex::Keyboard::Key key)
{
}

Player* World::GetPlayer()
{
	return m_Player;
}

BulletManager& World::GetBulletManager()
{
	return *m_BulletManager;
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