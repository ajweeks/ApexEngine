
#include "ApexMouse.h"

bool ApexMouse::s_Buttonsdown[sf::Mouse::ButtonCount];
bool ApexMouse::s_ButtonsdownLastFrame[sf::Mouse::ButtonCount];
bool ApexMouse::s_MouseMovedLastFrame = false;
sf::Vector2i ApexMouse::s_MousePosLastFrame;

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
		s_ButtonsdownLastFrame[i] = s_Buttonsdown[i];
		s_Buttonsdown[i] = sf::Mouse::isButtonPressed((sf::Mouse::Button)i);
	}
}

void ApexMouse::Clear()
{
	for (size_t i = 0; i < sf::Mouse::ButtonCount; ++i)
	{
		s_ButtonsdownLastFrame[i] = false;
		s_Buttonsdown[i] = false;
	}
}

bool ApexMouse::IsButtonDown(sf::Mouse::Button button)
{
	return s_Buttonsdown[button];
}

bool ApexMouse::IsButtonPressed(sf::Mouse::Button button)
{
	return s_Buttonsdown[button] && s_ButtonsdownLastFrame[button] == false;
}

bool ApexMouse::MovedLastFrame()
{
	return s_MousePosLastFrame != sf::Mouse::getPosition();
}

void ApexMouse::SetMousePosLastFrame()
{
	s_MousePosLastFrame = sf::Mouse::getPosition();
}

bool ApexMouse::IsButtonReleased(sf::Mouse::Button button)
{
	return s_Buttonsdown[button] == false && s_ButtonsdownLastFrame[button];
}
