
#include "ApexMouse.h"

bool ApexMouse::buttonsdown[sf::Mouse::ButtonCount];
bool ApexMouse::buttonsdownLastFrame[sf::Mouse::ButtonCount];

ApexMouse::ApexMouse()
{
}

ApexMouse::~ApexMouse()
{
}

void ApexMouse::Tick()
{
	for (size_t i = 0; i < sf::Mouse::ButtonCount; ++i)
	{
		buttonsdownLastFrame[i] = buttonsdown[i];
		buttonsdown[i] = sf::Mouse::isButtonPressed((sf::Mouse::Button)i);
	}
}

bool ApexMouse::IsButtonDown(sf::Mouse::Button button)
{
	return buttonsdown[button];
}

bool ApexMouse::IsButtonPressed(sf::Mouse::Button button)
{
	return buttonsdown[button] && buttonsdownLastFrame[button] == false;
}

bool ApexMouse::IsButtonReleased(sf::Mouse::Button button)
{
	return buttonsdown[button] == false && buttonsdownLastFrame[button];
}
