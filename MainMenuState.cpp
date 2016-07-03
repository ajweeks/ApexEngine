
#include "MainMenuState.h"
#include "ApexMain.h"
#include "enumerations.h"
#include "ApexKeyboard.h"
#include "ApexAudio.h"
#include "GameState.h"

#include <iostream>

MainMenuState::MainMenuState(StateManager* manager)
	: BaseState(manager, StateType::MAIN_MENU)
{
}

MainMenuState::~MainMenuState()
{
}

void MainMenuState::Tick(sf::Time elapsed)
{
}

void MainMenuState::Draw(sf::RenderTarget& target) const
{
	sf::RectangleShape bgRect(static_cast<sf::Vector2f>(sf::Vector2u(APEX->GetWindowSize().x, APEX->GetWindowSize().y)));
	bgRect.setFillColor(sf::Color(150, 55, 60));
	target.draw(bgRect);
}

bool MainMenuState::OnKeyPress(sf::Event::KeyEvent keyEvent, bool keyPressed)
{
	switch (keyEvent.code)
	{
	case sf::Keyboard::Space:
	{
		if (keyPressed)
		{
			m_StateManager->SetState(new GameState(m_StateManager));
			return true;
		}
	} break;
	}
	return false;
}

void MainMenuState::OnKeyRelease(sf::Event::KeyEvent keyEvent)
{
}
