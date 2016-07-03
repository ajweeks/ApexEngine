#pragma once

#include <SFML\Window\Event.hpp>

class KeyListener
{
public:
	KeyListener();
	virtual ~KeyListener();

	KeyListener(const KeyListener&) = delete;
	KeyListener& operator=(const KeyListener&) = delete;

	// If keypressed, this is the first frame the key was pressed (it was not held down)
	// Should return true if the key code should not be applied to any other listeners
	virtual bool OnKeyPress(sf::Event::KeyEvent keyEvent, bool keyPressed) = 0;
	virtual void OnKeyRelease(sf::Event::KeyEvent keyEvent) = 0;

private:

};
