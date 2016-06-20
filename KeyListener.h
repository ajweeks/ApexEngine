#pragma once

#include <SFML\Graphics.hpp>

class Game;

class KeyListener
{
public:
	KeyListener(Game* game);
	virtual ~KeyListener();

	KeyListener(const KeyListener&) = delete;
	KeyListener& operator=(const KeyListener&) = delete;

	virtual void OnKeyPress(sf::Event::KeyEvent keyEvent) = 0;
	virtual void OnKeyRelease(sf::Event::KeyEvent keyEvent) = 0;

private:
	Game* m_Game = nullptr;

};
