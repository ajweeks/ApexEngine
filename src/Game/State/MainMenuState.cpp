
#include "Game\State\MainMenuState.h"
#include "Game\State\GameState.h"
#include "Game\Game.h"
#include "Game\enumerations.h"

#include "Apex\ApexMain.h"
#include "Apex\Graphics\Button.h"
#include "Apex\enumerations.h"
#include "Apex\IO\Keyboard.h"
#include "Apex\IO\Audio.h"
#include "Apex\Graphics\TextureManager.h"

const sf::Color MainMenuState::BACKGROUND_BASE_COLOR = sf::Color(150, 55, 60);

MainMenuState::MainMenuState(apex::StateManager& stateManager)
	: apex::BaseState(int(StateType::MAIN_MENU), stateManager)
{
	float x = APEX->GetWindowSize().x / 2.0f;
	float y = 650;
	const float width = 400.0f;
	const float height = 125.0f;
	apex::Button* playButton = new apex::Button(x - width / 2.0f, y - height / 2.0f, width, height, "Play", Game::FontPixelFJ8);
	m_Buttons.AddButton(playButton, int(Buttons::PLAY));
	y += 150;
	apex::Button* quitButton = new apex::Button(x - width / 2.0f, y - height / 2.0f, width, height, "Quit", Game::FontPixelFJ8);
	m_Buttons.AddButton(quitButton, int(Buttons::QUIT));

	if (!m_VignetteShader.loadFromFile("resources/shaders/vignette.frag", sf::Shader::Fragment))
	{
		apex::PrintString("\n\n\t--Either couldn't find or couldn't compile vignette.frag--\n\n\n", apex::LogType::LOG_ERROR);
	}
	const sf::Vector2f windowSize = static_cast<sf::Vector2f>(APEX->GetWindowSize());
	m_VignetteShader.setUniform("u_resolution", windowSize);
	m_VignetteShader.setUniform("u_bounds", sf::Glsl::Vec4(0.0f, 0.0f, windowSize.x, windowSize.y));

	m_LogoSprite.setTexture(*apex::TextureManager::GetTexture(Texture::LIQWIDICE_GAMES_LOGO_SCREEN));

	m_TitleSprite.setTexture(*apex::TextureManager::GetTexture(Texture::TITLE_COLOR));
	const sf::Vector2u titleSize = m_TitleSprite.getTexture()->getSize();
	sf::Vector2f titlePos(windowSize.x / 2.0f - titleSize.x / 2.0f, 150.0f);
	m_TitleSprite.setPosition(titlePos);

	APEX->StartFadeOut();
}

MainMenuState::~MainMenuState()
{
}

void MainMenuState::Tick(sf::Time elapsed)
{
	const float time = APEX->GetTimeElapsed().asSeconds();
	const float x = (sin(time * 0.7658f) + 1.0f) / 2.0f;
	const float y = (cos(512.25f + time * 1.31f) + 1.0f) / 2.0f;
	m_VignetteShader.setUniform("u_center", sf::Glsl::Vec2(x / 4.0f + 0.3f, y / 6.0f + 0.4f));

	if (m_FadingOutTo != FadingOutTo::NONE)
	{
		switch (m_FadingOutTo)
		{
		case FadingOutTo::GAME:
		{
			if (!APEX->IsFadingIn())
			{
				m_StateManager.SetState(new GameState(m_StateManager));
			}
			return;
		} break;
		case FadingOutTo::QUIT:
		{
			if (!APEX->IsFadingIn())
			{
				APEX->Quit();
				return;
			}
		} break;
		default:
		{
		} break;
		}
	}

	if (m_FadingOutTo == FadingOutTo::NONE)
	{
		m_Buttons.Tick(elapsed);
		if (m_Buttons.GetButton(int(Buttons::PLAY))->IsPressed())
		{
			APEX->StartFadeIn();
			m_FadingOutTo = FadingOutTo::GAME;
			return;
		}

		if (m_Buttons.GetButton(int(Buttons::QUIT))->IsPressed())
		{
			APEX->StartFadeIn();
			m_FadingOutTo = FadingOutTo::QUIT;
			return;
		}
	}
}

void MainMenuState::Draw(sf::RenderTarget& target)
{
	const sf::Vector2f windowSize = static_cast<sf::Vector2f>(APEX->GetWindowSize());
	sf::RectangleShape bgRect(windowSize);
	bgRect.setFillColor(BACKGROUND_BASE_COLOR);
	target.draw(bgRect, &m_VignetteShader);
	m_Buttons.Draw(target, &m_VignetteShader);

	target.draw(m_TitleSprite);
}

bool MainMenuState::OnKeyPress(apex::Keyboard::Key key, bool keyPressed)
{
	return false;
}

void MainMenuState::OnKeyRelease(apex::Keyboard::Key key)
{
}
