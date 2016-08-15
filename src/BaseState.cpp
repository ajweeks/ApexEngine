
#include "BaseState.h"
#include "StateManager.h"
#include "enumerations.h"
#include "ApexKeyboard.h"

BaseState::BaseState(StateType stateType) : 
	ApexKeyListener(),
	m_StateType(stateType)
{
}

BaseState::~BaseState()
{
}

bool BaseState::OnKeyPress(ApexKeyboard::Key key, bool keyPressed)
{
	return false;
}

void BaseState::OnKeyRelease(ApexKeyboard::Key key)
{
}

StateType BaseState::GetType() const
{
	return m_StateType;
}
