
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
#include "ApexNPC.h"
#include "ApexMath.h"
#include "Interactable.h"
#include "Coin.h"

#include <fstream>

using namespace nlohmann;

const sf::Uint32 Level::MILLISECONDS_PER_SPEECH_BUBBLE_LETTER = 50;

Level::Level(int levelIndex) :
	ApexWindowListener(),
	m_LevelIndex(levelIndex),
	m_LightManager(levelIndex, this)
{
	m_DebugOverlay = new ApexDebug();
	m_ShowingDebugOverlay = false;

	m_BulletManager = new BulletManager();
	m_Map = new Map(this, "resources/level/" + std::to_string(levelIndex) + "/tiles.json", this);
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

	m_SpeechBubbleSpriteSheet = ApexSpriteSheet(TextureManager::GetTexture(TextureManager::SPEECH_BUBBLE), 16, 16);

	if (!m_OutlinedSpriteShader.loadFromFile("resources/shaders/outline_sprite.frag", sf::Shader::Fragment))
	{
		ApexOutputDebugString("Could not either load or compile color-sprite.frag\n");
	}
	m_OutlinedSpriteShader.setParameter("u_color", sf::Color(255, 255, 255));

	m_SpeechLetterTransition.SetEaseType(ApexTransition::EaseType::LINEAR);

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
	//m_Mobs.push_back(new Sheep(this, sf::Vector2f(400, 50)));

	json speechInfo = GetSpeechDataFromFile();
	std::vector<json> characters = speechInfo["characters"].get<std::vector<json>>();
	for (size_t i = 0; i < characters.size(); ++i)
	{
		m_Mobs.push_back(new ApexNPC(this, sf::Vector2f(128.0f + 16.0f * i, 228.0f), characters[i]));
	}
	
	for (size_t i = 0; i < m_Items.size(); i++)
	{
		delete m_Items[i];
	}
	m_Items.clear();
		
	m_Items.push_back(new Coin(this, sf::Vector2f(152, 150)));
	m_Items.push_back(new Coin(this, sf::Vector2f(100, 150)));
	m_Items.push_back(new Coin(this, sf::Vector2f(91, 250)));

	ClearSpeechShowing();
	m_HighlightedEntity = nullptr;
}

void Level::LoadShaders()
{
	m_LightManager.LoadShader();
}

void Level::LoadLights()
{
	m_LightManager.LoadLightData();
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

	// Somehow the player moved away while talking with someone, clear the speech bubble
	if (m_HighlightedEntity == nullptr && IsShowingSpeechBubble())
	{
		ClearSpeechShowing();
	}

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

void Level::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	sf::View cameraView = m_Camera->GetCurrentView();
	target.setView(cameraView);
	
	m_Map->DrawBackground(target, states);

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

	m_Map->DrawForeground(target, states);
	m_LightManager.Draw(target, states);

	// Static elements
	target.setView(target.getDefaultView());

	m_Minimap->Draw(target, states);

	m_HUD->Draw(target, states);

	if (!m_CurrentSpeech.empty())
	{
		const unsigned int SPEECH_FONT_SIZE = 48;
		const sf::Color SPEECH_FONT_COLOR = sf::Color(25, 25, 10);
		std::string speechString = m_CurrentSpeech.substr(0, m_SpeechLetterTransition.GetCurrentTransitionData() + 1);
		sf::Text speechText(m_CurrentSpeech, APEX->FontPixelFJ8, SPEECH_FONT_SIZE);
		speechText.setColor(SPEECH_FONT_COLOR);

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

void Level::ToggleLightingEditor()
{
	m_LightManager.ToggleShowingEditor();
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

bool Level::IsShowingSpeechBubble() const
{
	return !m_CurrentSpeech.empty();
}

void Level::SetCurrentSpeechShowing(const std::string& speech)
{
	m_CurrentSpeech = speech;

	const size_t stringLength = speech.length();
	m_SpeechLetterTransition.Create(0, stringLength, sf::milliseconds(MILLISECONDS_PER_SPEECH_BUBBLE_LETTER * stringLength));
	m_SpeechLetterTransition.Restart();

	m_Player->StopMoving();
}

void Level::ClearSpeechShowing()
{
	m_CurrentSpeech = "";
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

json Level::GetSpeechDataFromFile()
{
	json result;
	std::ifstream inputStream;

	inputStream.open("resources/speech.json");
	if (inputStream.is_open())
	{
		std::string line;
		std::stringstream stringStream;

		while (std::getline(inputStream, line))
		{
			stringStream << line;
		}
		inputStream.close();

		result = json::parse(stringStream.str());
	}

	return result;
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
