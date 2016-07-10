#pragma once

#include "BaseState.h"
#include <SFML\Graphics\ConvexShape.hpp>

class Level;

class GameState : public BaseState
{
public:
	GameState();
	virtual ~GameState();

	GameState(const GameState&) = delete;
	GameState& operator=(const GameState&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target);

	Level* GetLevel();
	bool IsLevelPaused() const;

	virtual bool OnKeyPress(sf::Event::KeyEvent keyEvent, bool keyPressed);
	virtual void OnKeyRelease(sf::Event::KeyEvent keyEvent);

	void Reset();

private:
	Level* m_Level = nullptr;

};

