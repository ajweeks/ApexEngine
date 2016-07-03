#pragma once

#include <SFML\Window\Mouse.hpp>

class ApexMouse
{
public:
	ApexMouse();
	virtual ~ApexMouse();

	static void Tick();
	static bool IsButtonDown(sf::Mouse::Button button);
	static bool IsButtonReleased(sf::Mouse::Button button);
	static bool IsButtonPressed(sf::Mouse::Button button);

private:
	static bool buttonsdown[sf::Mouse::ButtonCount];
	static bool buttonsdownLastFrame[sf::Mouse::ButtonCount];

};
