#pragma once

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include "enumerations.h"
#include "KeyListener.h"

class StateManager;

class BaseState : public KeyListener
{
public:
	BaseState(StateManager* manager, StateType stateType);
	virtual ~BaseState();

	BaseState(const BaseState&) = delete;
	BaseState& operator=(const BaseState&) = delete;

	virtual void Tick(sf::Time elapsed) = 0;
	virtual void Draw(sf::RenderTarget& target) const = 0;

	virtual bool OnKeyPress(sf::Event::KeyEvent keyEvent, bool keyPressed);
	virtual void OnKeyRelease(sf::Event::KeyEvent keyEvent);
	
	StateType GetType() const;

protected:
	StateType m_StateType;
	StateManager* m_StateManager = nullptr;

};
