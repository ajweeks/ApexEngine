
#include "Game\State\LoadingState.h"
#include "Game\State\GameState.h"
#include "Game\State\MainMenuState.h"
#include "Game\World\LightManager.h"
#include "Game\World\Map.h"
#include "Game\enumerations.h"

#include "Apex\ApexMain.h"
#include "Apex\IO\Keyboard.h"
#include "Apex\IO\Audio.h"
#include "Apex\Graphics\TextureManager.h"
#include "Apex\ApexMath.h"

LoadingState::LoadingState(apex::StateManager& stateManager)
	: BaseState(int(StateType::LOADING), stateManager)
{
	LightManager::LoadShaders();
	Map::LoadShaders();

	if (!Game::FontOpenSans.loadFromFile("resources/font/OpenSans/OpenSans-Regular.ttf"))
	{
		apex::PrintString("Couldn't load font OpenSans-Regular.ttf!\n", apex::LogType::LOG_WARNING);
	}
	if (!Game::FontPixelFJ8.loadFromFile("resources/font/pixelfj8/pixelFJ8.ttf"))
	{
		apex::PrintString("Couldn't load font pixelFJ8.ttf!\n", apex::LogType::LOG_WARNING);
	}

	apex::Audio::AddSoundEffect(size_t(Sound::GUN_FIRE), "resources/sound/gun-fire.wav");
	apex::Audio::AddSoundEffect(size_t(Sound::GUN_FIRE), "resources/sound/gun-fire.wav");
	apex::Audio::AddSoundEffect(size_t(Sound::GUN_FIRE_EMPTY), "resources/sound/gun-fire-empty.wav");
	apex::Audio::AddSoundEffect(size_t(Sound::GUN_RELOAD), "resources/sound/gun-reload.wav");
	apex::Audio::AddSoundEffect(size_t(Sound::BOOP), "resources/sound/boop.wav");
	apex::Audio::AddSoundEffect(size_t(Sound::BLIP), "resources/sound/blip.wav");
	apex::Audio::AddSoundEffect(size_t(Sound::COIN_PICKUP), "resources/sound/coin-pickup.wav");
	apex::Audio::AddSoundEffect(size_t(Sound::TYPING_1), "resources/sound/typing-1.wav");
	apex::Audio::AddSoundEffect(size_t(Sound::TYPING_2), "resources/sound/typing-2.wav");
	apex::Audio::AddSoundEffect(size_t(Sound::TYPING_3), "resources/sound/typing-3.wav");

	// NOTE: Add songs here:


	// TODO: Call into texture manager to resize? (slight performace increase)
	apex::TextureManager::AddTexture(size_t(Texture::LIQWIDICE_GAMES_LOGO_SCREEN), "resources/images/title-card.png");
	apex::TextureManager::AddTexture(size_t(Texture::GENERAL_TILES), "resources/images/tiles.png");
	apex::TextureManager::AddTexture(size_t(Texture::PLAYER), "resources/images/player.png");
	apex::TextureManager::AddTexture(size_t(Texture::DUST_PARTICLE), "resources/images/dust-explosion.png");
	apex::TextureManager::AddTexture(size_t(Texture::SHADOW), "resources/images/shadow.png");
	apex::TextureManager::AddTexture(size_t(Texture::AMMO), "resources/images/ammo.png");
	apex::TextureManager::AddTexture(size_t(Texture::COIN), "resources/images/coin.png");
	apex::TextureManager::AddTexture(size_t(Texture::TITLE_COLOR), "resources/images/title.png");
	apex::TextureManager::AddTexture(size_t(Texture::SPEECH_BUBBLE), "resources/images/speech-bubble.png");
	apex::TextureManager::AddTexture(size_t(Texture::AI_CHARACTER), "resources/images/npc.png");
	apex::TextureManager::AddTexture(size_t(Texture::BUILDINGS), "resources/images/buildings.png");

	//apex::TextureManager::AddTexture(size_t(Texture::CURSOR_NORMAL), "resources/images/cursor-normal-x2.png");
	apex::TextureManager::AddTexture(size_t(Texture::CURSOR_POINTER), "resources/images/cursor-point-x2.png");

	apex::Keyboard::SetKeybindingsFilePath("resources/keybindings.json");
	apex::Keyboard::LoadKeybindingsFromFile();
	AddKeybinding(Key::MOVE_LEFT, sf::Keyboard::A);
	AddKeybinding(Key::MOVE_RIGHT, sf::Keyboard::D);
	AddKeybinding(Key::MOVE_UP, sf::Keyboard::W);
	AddKeybinding(Key::MOVE_DOWN, sf::Keyboard::S);

	AddKeybinding(Key::PAUSE, sf::Keyboard::Escape);
	AddKeybinding(Key::INTERACT, sf::Keyboard::E);
	AddKeybinding(Key::SCREENSHOT, sf::Keyboard::F10);

	AddKeybinding(Key::DEBUG_TOGGLE_LIGHT_EDITOR, sf::Keyboard::F8);
	AddKeybinding(Key::DEBUG_TOGGLE_INFO_OVERLAY, sf::Keyboard::F9);
	AddKeybinding(Key::DEBUG_TOGGLE_PHYSICS_OVERLAY, sf::Keyboard::P);
	AddKeybinding(Key::DEBUG_STEP_ONE_PHYSICS_FRAME, sf::Keyboard::Period);
	AddKeybinding(Key::DEBUG_PAUSE_EVERYTHING, sf::Keyboard::Space);
	AddKeybinding(Key::DEBUG_RESTART, sf::Keyboard::R);
	apex::Keyboard::LoadKeybindingsFromFile();

	//APEX->AddCursorType(apex::Cursor::NORMAL, apex::TextureManager::GetTexture(Texture::CURSOR_NORMAL));
	//APEX->AddCursorType(apex::Cursor::POINT, apex::TextureManager::GetTexture(Texture::CURSOR_POINTER));


	m_LogoSprite.setTexture(*apex::TextureManager::GetTexture(Texture::LIQWIDICE_GAMES_LOGO_SCREEN));
	
	sf::Color start = sf::Color::Black;
	sf::Color end = sf::Color::White;
	m_FadeTransition.Create(start, end, sf::seconds(0.75f), apex::Transition::EaseType::QUADRATIC_OUT);

	const sf::Vector2u windowSize = APEX->GetWindowSize();
	const sf::Vector2u imageSize = m_LogoSprite.getTexture()->getSize();
	m_LogoSprite.setScale(windowSize.x / float(imageSize.x), windowSize.y / float(imageSize.y));

	m_BufferTime = sf::seconds(0.4f);
	m_TimeToShowLogo = sf::seconds(2.5f);

	m_LogoSprite.setColor(sf::Color::Black);

	m_AnimState = AnimationState::INITAL_BLACK;
	m_SpriteFlicker = 1.0f;
	m_LogoOffset = sf::Vector2f(0.0f, 0.0f);
}

inline void LoadingState::AddKeybinding(Key key, int defaultVKCode)
{
	const size_t keyIndex = size_t(key);
	apex::Keyboard::AddKeybinding(keyIndex, Game::GetKeyName(keyIndex), defaultVKCode);
}

LoadingState::~LoadingState()
{
}

void LoadingState::Tick(sf::Time elapsed)
{
	const sf::Vector2f windowSize = static_cast<sf::Vector2f>(APEX->GetWindowSize());
	sf::Vector2f diff;
	if (APEX->IsMouseInWindow())
	{
		const sf::Vector2f mousePos = static_cast<sf::Vector2f>(APEX->GetMouseCoordsScreenSpace());
		diff = (mousePos - windowSize / 2.0f) - m_LogoOffset;
	}
	else
	{
		diff = sf::Vector2f(0.0f, 0.0f) - m_LogoOffset;
	}
	m_LogoOffset += diff / 500.0f;
	m_LogoSprite.setPosition(m_LogoOffset);
	
	switch (m_AnimState)
	{
	case AnimationState::INITAL_BLACK:
	{
		if (m_BufferTime > sf::Time::Zero)
		{
			m_BufferTime -= elapsed;
			return;
		}

		m_AnimState = AnimationState::FADE_IN;
	} break;
	case AnimationState::FADE_IN:
	{
		m_FadeTransition.Tick(elapsed);
		if (m_FadeTransition.GetPercentComplete() >= 1.0f)
		{
			m_AnimState = AnimationState::HOLD;
			m_SpriteFlicker = 0.95f;
		}
		else
		{
			m_SpriteFlicker = (std::rand() % 50) / 100.0f + 0.5f;
		}
	} break;
	case AnimationState::HOLD:
	{
		m_TimeToShowLogo -= elapsed;
		if (m_TimeToShowLogo <= sf::Time::Zero)
		{
			m_FadeTransition.SwapAndRestart();
			m_AnimState = AnimationState::FADE_OUT;
		}
		else
		{
			if (m_SpriteFlicker > 0.95f)
			{
				m_SpriteFlicker += apex::ApexMath::Random(-0.05f, -0.01f);
			}
			else if (m_SpriteFlicker < 0.5f)
			{
				m_SpriteFlicker += apex::ApexMath::Random(0.01f, 0.05f);
			}
			else
			{
				m_SpriteFlicker += apex::ApexMath::Random(0.02f, 0.04f) * (std::rand() % 2 == 0 ? 1 : -1);
			}
		}
	} break;
	case AnimationState::FADE_OUT:
	{
		m_FadeTransition.Tick(elapsed);
		if (m_FadeTransition.GetPercentComplete() >= 1.0f)
		{
			m_AnimState = AnimationState::FINAL_BLACK;
			m_BufferTime = sf::seconds(0.5f);
		}
		else
		{
			m_SpriteFlicker = (std::rand() % 50) / 100.0f + 0.5f;
		}
	} break;
	case AnimationState::FINAL_BLACK:
	{
		m_BufferTime -= elapsed;
		if (m_BufferTime <= sf::Time::Zero)
		{
			MainMenuState* mainMenuState = new MainMenuState(m_StateManager);
			 m_StateManager.SetState(mainMenuState);
			return;
		}
	} break;
	}

	sf::Color spriteColor = m_FadeTransition.GetCurrentColor();
	spriteColor.a = sf::Uint8(m_SpriteFlicker * 127 + 128);
	m_LogoSprite.setColor(spriteColor);
}

void LoadingState::Draw(sf::RenderTarget& target)
{
	target.draw(m_LogoSprite);
}

bool LoadingState::OnKeyPress(apex::Keyboard::Key key, bool keyPressed)
{
	switch (Key(key.vkCode))
	{
	case Key::PAUSE:
	case Key::INTERACT:
		m_AnimState = AnimationState::FINAL_BLACK;
		m_BufferTime = sf::Time::Zero;
		break;
	}
	return false;
}

void LoadingState::OnKeyRelease(apex::Keyboard::Key key)
{
}
