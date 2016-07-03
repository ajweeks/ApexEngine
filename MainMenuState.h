#pragma once

#include "BaseState.h"
#include "ApexMain.h"

class MainMenuState : public BaseState
{
public:
	MainMenuState(StateManager* manager);
	virtual ~MainMenuState();

	MainMenuState(const MainMenuState&) = delete;
	MainMenuState& operator=(const MainMenuState&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target) const;

	virtual bool OnKeyPress(sf::Event::KeyEvent keyEvent, bool keyPressed);
	virtual void OnKeyRelease(sf::Event::KeyEvent keyEvent);

private:

};

