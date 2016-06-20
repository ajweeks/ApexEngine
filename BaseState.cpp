
#include "BaseState.h"
#include "StateManager.h"
#include "enumerations.h"

BaseState::BaseState(StateManager* manager, StateType stateType, Game* game)
	: m_StateManager(manager), m_StateType(stateType), m_Game(game)
{
}

BaseState::~BaseState()
{
}

StateType BaseState::GetType() const
{
	return m_StateType;
}
