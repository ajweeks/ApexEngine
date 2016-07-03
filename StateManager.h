#pragma once

#include <vector>
#include "BaseState.h"
#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

class StateManager
{
public:
	StateManager(BaseState* startingState = nullptr);
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

