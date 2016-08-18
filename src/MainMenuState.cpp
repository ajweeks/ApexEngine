
#include "MainMenuState.h"
#include "ApexMain.h"
#include "ApexButton.h"
#include "enumerations.h"
#include "ApexKeyboard.h"
#include "ApexAudio.h"
#include "GameState.h"
#include "TextureManager.h"

MainMenuState::MainMenuState()
	: BaseState(StateType::MAIN_MENU)
{
	float x = APEX->GetWindowSize().x / 2.0f;
	float y = 650;
	const float width = 400.0f;
	const float height = 125.0f;
	ApexButton* playButton = new ApexButton(x - width / 2.0f, y - height / 2.0f, width, height, "Play");
	m_Buttons.AddButton(playButton, int(Buttons::PLAY));
	y += 150;
	ApexButton* quitButton = new ApexButton(x - width / 2.0f, y - height / 2.0f, width, height, "Quit");
	m_Buttons.AddButton(quitButton, int(Buttons::QUIT));

	if (!m_VignetteShader.loadFromFile("resources/shaders/vignette.frag", sf::Shader::Fragment))
	{
		ApexOutputDebugString("\n\n\t--Either couldn't find or couldn't compile vignette.frag--\n\n\n");
	}
	const sf::Vector2f windowSize = static_cast<sf::Vector2f>(APEX->GetWindowSize());
	m_VignetteShader.setParameter("u_resolution", windowSize);
	m_VignetteShader.setParameter("u_bounds", 0.0f, 0.0f, windowSize.x, windowSize.y);

	m_LogoSprite.setTexture(*TextureManager::GetTexture(TextureManager::LIQWIDICE_GAMES_LOGO_SCREEN));

	m_TitleColorSprite.setTexture(*TextureManager::GetTexture(TextureManager::TITLE_COLOR));
	m_TitleShadowSprite.setTexture(*TextureManager::GetTexture(TextureManager::TITLE_SHADOW));
	const sf::Vector2u titleSize = m_TitleColorSprite.getTexture()->getSize();
	sf::Vector2f titlePos(windowSize.x / 2.0f - titleSize.x / 2.0f, 150.0f);
	m_TitleColorSprite.setPosition(titlePos);
	titlePos.y += 1.5f;
	m_TitleShadowSprite.setPosition(titlePos);

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
	m_VignetteShader.setParameter("u_center", x / 4.0f + 0.3f, y / 6.0f + 0.4f);

	if (APEX->IsFadingOut())
	{
		switch (m_FadingOutTo)
		{
		case FadingOutTo::GAME:
		{
			APEX->GetStateManager()->SetState(new GameState());
			return;
		} break;
		case FadingOutTo::QUIT:
		{
			APEX->Quit();
			return;
		} break;
		case FadingOutTo::NONE:
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
			APEX->StartFadeInOut(sf::seconds(0.5f));
			m_FadingOutTo = FadingOutTo::GAME;
			return;
		}

		if (m_Buttons.GetButton(int(Buttons::QUIT))->IsPressed())
		{
			APEX->StartFadeInOut(sf::seconds(0.4f));
			m_FadingOutTo = FadingOutTo::QUIT;
			return;
		}

		const float secondsElapsed = APEX->GetTimeElapsed().asSeconds();
		m_TitleShadowSprite.move(0.0f, sin(secondsElapsed * 0.8f) * 0.065f);
	}
}

void MainMenuState::Draw(sf::RenderTarget& target)
{
	const sf::Vector2f windowSize = static_cast<sf::Vector2f>(APEX->GetWindowSize());
	sf::RectangleShape bgRect(windowSize);
	bgRect.setFillColor(sf::Color(150, 55, 60));
	target.draw(bgRect, &m_VignetteShader);
	m_Buttons.Draw(target, &m_VignetteShader);

	target.draw(m_TitleShadowSprite);
	target.draw(m_TitleColorSprite);
}

bool MainMenuState::OnKeyPress(ApexKeyboard::Key key, bool keyPressed)
{
	return false;
}

void MainMenuState::OnKeyRelease(ApexKeyboard::Key key)
{
}