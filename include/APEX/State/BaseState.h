#pragma once

#include "Apex\enumerations.h"
#include "Apex\IO\KeyListener.h"
#include "Apex\IO\Keyboard.h"

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

namespace apex
{
	class StateManager;

	class BaseState : public KeyListener
	{
	public:
		BaseState(int index, StateManager& stateManager);
		virtual ~BaseState();

		BaseState(const BaseState&) = delete;
		BaseState& operator=(const BaseState&) = delete;

		virtual void Tick(sf::Time elapsed) = 0;
		virtual void Draw(sf::RenderTarget& target) = 0;

		virtual bool OnKeyPress(Keyboard::Key key, bool keyPressed);
		virtual void OnKeyRelease(Keyboard::Key key);

		int GetIndex() const;
		StateManager& GetStateManager();

	protected:
		int m_Index;
		StateManager& m_StateManager;

	};
} // namespace apex