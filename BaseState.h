#pragma once

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include "enumerations.h"
#include "ApexKeyListener.h"

class StateManager;

class BaseState : public ApexKeyListener
{
public:
	BaseState(StateManager* manager, StateType stateType);
	virtual ~BaseState();

	BaseState(const BaseState&) = delete;
	BaseState& operator=(const BaseState&) = delete;

	virtual void Tick(sf::Time elapsed) = 0;
	virtual void Draw(sf::RenderTarget& target) = 0;

	virtual bool OnKeyPress(sf::Event::KeyEvent keyEvent, bool keyPressed);
	virtual void OnKeyRelease(sf::Event::KeyEvent keyEvent);
	
	StateType GetType() const;

protected:
	StateType m_StateType;
	StateManager* m_StateManager = nullptr;

};
