
#include <APEX\State\BaseState.h>
#include <APEX\State\StateManager.h>
#include <APEX\enumerations.h>
#include <APEX\IO\Keyboard.h>

namespace apex
{
	BaseState::BaseState(int index, StateManager& stateManager) :
		KeyListener(),
		m_Index(index),
		m_StateManager(stateManager)
	{
	}

	BaseState::~BaseState()
	{
	}

	bool BaseState::OnKeyPress(Keyboard::Key key, bool keyPressed)
	{
		return false;
	}

	void BaseState::OnKeyRelease(Keyboard::Key key)
	{
	}

	int BaseState::GetIndex() const
	{
		return m_Index;
	}

	StateManager& BaseState::GetStateManager()
	{
		return m_StateManager;
	}
} // namespace apex
