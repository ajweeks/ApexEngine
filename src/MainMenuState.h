#pragma once

#include "BaseState.h"
#include "ApexMain.h"
#include "ApexButtonList.h"
#include "TransformableTransition.h"

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

	virtual bool OnKeyPress(ApexKeyboard::Key key, bool keyPressed) override;
	virtual void OnKeyRelease(ApexKeyboard::Key key) override;

private:
	enum class Buttons
	{
		PLAY, QUIT
	};
	enum class FadingOutTo
	{
		GAME, QUIT, NONE
	};

	ApexButtonList m_Buttons;

	sf::Shader m_VignetteShader;
	sf::Sprite m_TitleColorSprite;
	sf::Sprite m_TitleShadowSprite;

	sf::Sprite m_LogoSprite;

	FadingOutTo m_FadingOutTo = FadingOutTo::NONE;
};

