
#include "Apex\State\StateManager.h"

namespace apex
{
	StateManager::StateManager()
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
		if (m_CurrentState != nullptr) delete m_CurrentState;
		m_CurrentState = newState;
	}

	BaseState* StateManager::CurrentState()
	{
		return m_CurrentState;
	}
} // namespace apex