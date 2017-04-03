#pragma once

#include "Game\Game.h"
#include "Game\ButtonList.h"

#include "Apex\State\BaseState.h"
#include "Apex\Transition\TransformableTransition.h"

#include <SFML\Graphics\Shader.hpp>

class MainMenuState : public apex::BaseState
{
public:
	MainMenuState(apex::StateManager& stateManager);
	virtual ~MainMenuState();

	MainMenuState(const MainMenuState&) = delete;
	MainMenuState& operator=(const MainMenuState&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target);

	virtual bool OnKeyPress(apex::Keyboard::Key key, bool keyPressed) override;
	virtual void OnKeyRelease(apex::Keyboard::Key key) override;

private:
	enum class Buttons
	{
		PLAY, QUIT
	};
	enum class FadingOutTo
	{
		GAME, QUIT, NONE
	};

	static const sf::Color BACKGROUND_BASE_COLOR;

	ButtonList m_Buttons;

	sf::Shader m_VignetteShader;
	sf::Sprite m_TitleSprite;

	sf::Sprite m_LogoSprite;

	FadingOutTo m_FadingOutTo = FadingOutTo::NONE;
};

