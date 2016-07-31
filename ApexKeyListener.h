#pragma once

#include "ApexKeyboard.h"

#include <SFML\Window\Event.hpp>

class ApexKeyListener
{
public:
	ApexKeyListener();
	virtual ~ApexKeyListener();

	ApexKeyListener(const ApexKeyListener&) = delete;
	ApexKeyListener& operator=(const ApexKeyListener&) = delete;

	// If keypressed, this is the first frame the key was pressed (it was not held down)
	// Returns whether or not to block further input
	// (Should return true if the key code should not be applied to any other listeners)
	virtual bool OnKeyPress(ApexKeyboard::Key key, bool keyPressed) = 0;
	virtual void OnKeyRelease(ApexKeyboard::Key key) = 0;

private:

};
