
#include "BaseState.h"
#include "StateManager.h"
#include "enumerations.h"

BaseState::BaseState(StateManager* manager, StateType stateType)
	: m_StateManager(manager), m_StateType(stateType)
{
}

BaseState::~BaseState()
{
}

StateType BaseState::GetType() const
{
	return m_StateType;
}
