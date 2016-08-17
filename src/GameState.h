#pragma once

#include "BaseState.h"

#include <SFML\Graphics\ConvexShape.hpp>

class World;

class GameState : public BaseState
{
public:
	GameState();
	virtual ~GameState();

	GameState(const GameState&) = delete;
	GameState& operator=(const GameState&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target);

	World* GetWorld();
	bool IsWorldPaused() const;

	virtual bool OnKeyPress(ApexKeyboard::Key key, bool keyPressed) override;
	virtual void OnKeyRelease(ApexKeyboard::Key key) override;

	void OnUnmappedKeypress(sf::Event::KeyEvent event);

	void Reset();

private:
	// LATER: Add more worlds? (underworld, aether, ...)
	World* m_World = nullptr;

};

