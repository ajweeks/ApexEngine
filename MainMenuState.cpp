
#include "MainMenuState.h"
#include "ApexMain.h"
#include "enumerations.h"
#include "ApexKeyboard.h"
#include "ApexAudio.h"
#include "GameState.h"

MainMenuState::MainMenuState()
	: BaseState(StateType::MAIN_MENU)
{
	float x = APEX->GetWindowSize().x / 2.0f;
	float y = 650;
	const float width = 400.0f;
	const float height = 125.0f;
	m_PlayButton = new ApexButton(x - width / 2.0f, y - height / 2.0f, width, height, "Play");
	y += 150;
	m_QuitButton = new ApexButton(x - width / 2.0f, y - height / 2.0f, width, height, "Quit");

	if (!m_VignetteShader.loadFromFile("resources/shaders/vignette.frag", sf::Shader::Fragment))
	{
		ApexOutputDebugString("\n\n\t--Either couldn't find or couldn't compile vignette.frag--\n\n\n");
	}
	const sf::Vector2f windowSize = static_cast<sf::Vector2f>(APEX->GetWindowSize());
	m_VignetteShader.setParameter("u_resolution", windowSize);
	m_VignetteShader.setParameter("u_bounds", 0.0f, 0.0f, windowSize.x, windowSize.y);
}

MainMenuState::~MainMenuState()
{
	delete m_PlayButton;
	delete m_QuitButton;
}

void MainMenuState::Tick(sf::Time elapsed)
{
	const float time = APEX->GetTimeElapsed().asSeconds();
	const float x = (sin(time * 0.7658f) + 1.0f) / 2.0f;
	const float y = (cos(512.25f + time * 1.31f) + 1.0f) / 2.0f;
	m_VignetteShader.setParameter("u_center", x / 4.0f + 0.3f, y / 6.0f + 0.4f);

	m_PlayButton->Tick(elapsed);
	if (m_PlayButton->IsDown())
	{
		APEX->GetStateManager()->SetState(new GameState());
		return;
	}

	m_QuitButton->Tick(elapsed);
	if (m_QuitButton->IsDown())
	{
		APEX->Quit();
		return;
	}
}

void MainMenuState::Draw(sf::RenderTarget& target)
{
	sf::RectangleShape bgRect(static_cast<sf::Vector2f>(sf::Vector2u(APEX->GetWindowSize().x, APEX->GetWindowSize().y)));
	bgRect.setFillColor(sf::Color(150, 55, 60));
	target.draw(bgRect, &m_VignetteShader);
	m_PlayButton->Draw(target, &m_VignetteShader);
	m_QuitButton->Draw(target, &m_VignetteShader);
}

bool MainMenuState::OnKeyPress(sf::Event::KeyEvent keyEvent, bool keyPressed)
{
	return false;
}

void MainMenuState::OnKeyRelease(sf::Event::KeyEvent keyEvent)
{
}
