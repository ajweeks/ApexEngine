#pragma once


#include "enumerations.h"
#include "ApexKeyListener.h"
#include "ApexKeyboard.h"

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

class StateManager;

class BaseState : public ApexKeyListener
{
public:
	BaseState(StateType stateType);
	virtual ~BaseState();

	BaseState(const BaseState&) = delete;
	BaseState& operator=(const BaseState&) = delete;

	virtual void Tick(sf::Time elapsed) = 0;
	virtual void Draw(sf::RenderTarget& target) = 0;

	virtual bool OnKeyPress(ApexKeyboard::Key key, bool keyPressed);
	virtual void OnKeyRelease(ApexKeyboard::Key key);
	
	StateType GetType() const;

protected:
	StateType m_StateType;

};
