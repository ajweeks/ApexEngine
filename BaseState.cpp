
#include "BaseState.h"
#include "StateManager.h"
#include "enumerations.h"

BaseState::BaseState(StateManager* manager, StateType stateType) : 
	ApexKeyListener(),
	m_StateManager(manager), m_StateType(stateType)
{
}

BaseState::~BaseState()
{
}

bool BaseState::OnKeyPress(sf::Event::KeyEvent keyEvent, bool keyPressed)
{
	return false;
}

void BaseState::OnKeyRelease(sf::Event::KeyEvent keyEvent)
{
}

StateType BaseState::GetType() const
{
	return m_StateType;
}
