#pragma once

#include "Game\Game.h"

#include "Apex\State\BaseState.h"
#include "Apex\Graphics\Button.h"
#include "Apex\Transition\ColorTransition.h"

#include <SFML\Graphics\Shader.hpp>

class LoadingState : public apex::BaseState
{
public:
	LoadingState(apex::StateManager& stateManager);
	virtual ~LoadingState();

	LoadingState(const LoadingState&) = delete;
	LoadingState& operator=(const LoadingState&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target);

	virtual bool OnKeyPress(apex::Keyboard::Key key, bool keyPressed) override;
	virtual void OnKeyRelease(apex::Keyboard::Key key) override;

private:
	enum class AnimationState
	{
		INITAL_BLACK, FADE_IN, HOLD, FADE_OUT, FINAL_BLACK
	};
	void AddKeybinding(Key key, int defaultVKCode);

	sf::Sprite m_LogoSprite;

	apex::ColorTransition m_FadeTransition;
	sf::Time m_TimeToShowLogo;
	sf::Time m_BufferTime;
	AnimationState m_AnimState;
	float m_SpriteFlicker;
	
	sf::Vector2f m_LogoOffset;

};

