#pragma once

#include "BaseState.h"
#include "ApexMain.h"
#include "ApexButton.h"
#include "TransformationTransition.h"

#include <SFML\Graphics\Shader.hpp>

class LoadingState : public BaseState
{
public:
	LoadingState();
	virtual ~LoadingState();

	LoadingState(const LoadingState&) = delete;
	LoadingState& operator=(const LoadingState&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target);

	virtual bool OnKeyPress(ApexKeyboard::Key key, bool keyPressed) override;
	virtual void OnKeyRelease(ApexKeyboard::Key key) override;

private:
	enum class AnimationState
	{
		INITAL_BLACK, FADE_IN, HOLD, FADE_OUT, FINAL_BLACK
	};

	sf::Sprite m_LogoSprite;

	TransformationTransition m_FadeTransition;
	sf::Time m_TimeToShowLogo;
	sf::Time m_BufferTime;
	AnimationState m_AnimState;
	float m_SpriteFlicker;

};

