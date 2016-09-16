#pragma once

#include "ApexKeyboard.h"

#include <SFML\Window\Event.hpp>

class ApexKeyListener
{
public:
	// Set priority to a lower value for higher priority, or -1 for default
	// Key listeners added with a priority of 0 are added to the front of the list
	//										  1 are added after the first element
	//										  etc.
	ApexKeyListener(int priority = -1);
	virtual ~ApexKeyListener();

	ApexKeyListener(const ApexKeyListener&) = delete;
	ApexKeyListener& operator=(const ApexKeyListener&) = delete;

	// If keypressed, this is the first frame the key was pressed (it was not held down)
	// Returns whether or not to block further input
	// (Should return true if the key code should not be applied to any other listeners)
	virtual bool OnKeyPress(ApexKeyboard::Key key, bool keyPressed) { return false; };
	virtual bool OnUnmappedKeyPress(int vkCode, bool keyPressed) { return false; };
	virtual void OnKeyRelease(ApexKeyboard::Key key) {};

private:

};
