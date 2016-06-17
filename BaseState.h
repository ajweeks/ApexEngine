#pragma once

#include <SFML\Graphics.hpp>

class StateManager;
enum class StateType;

class BaseState
{
public:
	BaseState(StateManager* manager, StateType stateType);
	virtual ~BaseState();

	BaseState(const BaseState&) = delete;
	BaseState& operator=(const BaseState&) = delete;

	virtual void Tick(sf::Time elapsed) = 0;
	virtual void Draw(sf::RenderTarget& target) const = 0;

	StateType GetType() const;

protected:
	StateManager* m_StateManager = nullptr;
	StateType m_StateType;

};
