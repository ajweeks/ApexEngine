#pragma once

#include "BaseState.h"
#include <SFML\Graphics\ConvexShape.hpp>

class Level;

// TODO: Add OnKeyPress/OnMouseButton functions to all states?

class GameState : public BaseState
{
public:
	GameState(StateManager* manager);
	virtual ~GameState();

	GameState(const GameState&) = delete;
	GameState& operator=(const GameState&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target);
	void TogglePaused(bool pauseSounds);
	Level* GetLevel();

	virtual bool OnKeyPress(sf::Event::KeyEvent keyEvent, bool keyPressed);
	virtual void OnKeyRelease(sf::Event::KeyEvent keyEvent);

	void Reset();

private:
	sf::ConvexShape CreateStar(sf::Vector2f centerPos, size_t numPoints, float innerRadius, float outerRadius);

	Level* m_Level = nullptr;
	bool m_Paused = false;

};

