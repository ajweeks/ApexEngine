
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

	m_VignetteTexture.loadFromFile("resources/vig.png");
	m_VignetteSprite.setTexture(m_VignetteTexture);
}

MainMenuState::~MainMenuState()
{
	delete m_PlayButton;
	delete m_QuitButton;
}

void MainMenuState::Tick(sf::Time elapsed)
{
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
	target.draw(bgRect);

	m_PlayButton->Draw(target, sf::RenderStates::Default);
	m_QuitButton->Draw(target, sf::RenderStates::Default);

	sf::RenderStates vigStates;
	vigStates.blendMode = sf::BlendMultiply;
	target.draw(m_VignetteSprite, vigStates);
}

bool MainMenuState::OnKeyPress(sf::Event::KeyEvent keyEvent, bool keyPressed)
{
	return false;
}

void MainMenuState::OnKeyRelease(sf::Event::KeyEvent keyEvent)
{
}
