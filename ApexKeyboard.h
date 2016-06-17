#pragma once

#include <SFML\Graphics.hpp>

class ApexKeyboard
{
public:
	ApexKeyboard();
	virtual ~ApexKeyboard();

	static void Tick();
	static bool IsKeyDown(const sf::Keyboard::Key& key);
	static bool IsKeyPressed(const sf::Keyboard::Key& key);
	static bool IsKeyReleased(const sf::Keyboard::Key& key);

private:
	static bool keysdown[sf::Keyboard::KeyCount];
	static bool keysdownLastFrame[sf::Keyboard::KeyCount];
};
