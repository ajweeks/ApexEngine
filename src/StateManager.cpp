
#include "StateManager.h"

StateManager::StateManager(BaseState* startingState)
	: m_CurrentState(startingState)
{
}

StateManager::~StateManager()
{
	delete m_CurrentState;
}

void StateManager::Tick(sf::Time elapsed)
{
	m_CurrentState->Tick(elapsed);
}

void StateManager::Draw(sf::RenderTarget& target) const
{
	m_CurrentState->Draw(target);
}

void StateManager::SetState(BaseState* newState)
{
	delete m_CurrentState;
	m_CurrentState = newState;
}

BaseState* StateManager::CurrentState()
{
	return m_CurrentState;
}
