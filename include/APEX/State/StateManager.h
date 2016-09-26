#pragma once

#include <APEX\State\BaseState.h>

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

#include <vector>

namespace apex
{
	class StateManager
	{
	public:
		StateManager();
		virtual ~StateManager();

		StateManager(const StateManager&) = delete;
		StateManager& operator=(const StateManager&) = delete;

		void Tick(sf::Time elapsed);
		void Draw(sf::RenderTarget& target) const;

		void SetState(BaseState* newState);
		BaseState* CurrentState();

	private:
		BaseState* m_CurrentState = nullptr;

	};
} // namespace apex
