#pragma once

#include <APEX\IO\Keyboard.h>

#include <SFML\Window\Event.hpp>

namespace apex
{
	class KeyListener
	{
	public:
		// Set priority to a lower value for higher priority, or -1 for default
		// Key listeners added with a priority of 0 are added to the front of the list
		//										  1 are added after the first element
		//										  etc.
		KeyListener(int priority = -1);
		virtual ~KeyListener();

		KeyListener(const KeyListener&) = delete;
		KeyListener& operator=(const KeyListener&) = delete;

		// If keypressed, this is the first frame the key was pressed (it was not held down)
		// Returns whether or not to block further input
		// (Should return true if the key code should not be applied to any other listeners)
		virtual bool OnKeyPress(Keyboard::Key key, bool keyPressed) { return false; };
		virtual bool OnUnmappedKeyPress(int vkCode, bool keyPressed) { return false; };
		virtual void OnKeyRelease(Keyboard::Key key) {};

	private:

	};
} // namespace apex
