#pragma once

#include "BaseState.h"
#include "Game.h"

class Level;


// TODO: Add OnKeyPress/OnMouseButton functions to all states?


class GameState : public BaseState
{
public:
	GameState(StateManager* manager, Game* game);
	virtual ~GameState();

	GameState(const GameState&) = delete;
	GameState& operator=(const GameState&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target) const;
	void TogglePaused(bool pauseSounds);
	Level* GetLevel();

	void Reset();

private:
	sf::ConvexShape CreateStar(sf::Vector2f centerPos, size_t numPoints, float innerRadius, float outerRadius);

	Level* m_Level = nullptr;
	bool m_Paused = false;

};

