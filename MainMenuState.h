#pragma once

#include "BaseState.h"
#include "ApexMain.h"
#include "ApexButton.h"

#include <SFML\Graphics\Shader.hpp>

class MainMenuState : public BaseState
{
public:
	MainMenuState();
	virtual ~MainMenuState();

	MainMenuState(const MainMenuState&) = delete;
	MainMenuState& operator=(const MainMenuState&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target);

	virtual bool OnKeyPress(sf::Event::KeyEvent keyEvent, bool keyPressed);
	virtual void OnKeyRelease(sf::Event::KeyEvent keyEvent);

private:
	ApexButton* m_PlayButton = nullptr;
	ApexButton* m_QuitButton = nullptr;

	sf::Shader m_VignetteShader;

};

