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

	virtual bool OnKeyPress(ApexKeyboard::Key key, bool keyPressed) override;
	virtual void OnKeyRelease(ApexKeyboard::Key key) override;

	void OnUnmappedKeypress(sf::Event::KeyEvent event);

	void Reset();

private:
	Level* m_Level = nullptr;

};

