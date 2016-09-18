
#include <APEX\IO\Mouse.h>

namespace apex
{
	bool Mouse::s_Buttonsdown[sf::Mouse::ButtonCount];
	bool Mouse::s_ButtonsdownLastFrame[sf::Mouse::ButtonCount];
	bool Mouse::s_MouseMovedLastFrame = false;
	sf::Vector2i Mouse::s_MousePosLastFrame;

	Mouse::Mouse()
	{
	}

	Mouse::~Mouse()
	{
	}

	void Mouse::Tick()
	{
		for (size_t i = 0; i < sf::Mouse::ButtonCount; ++i)
		{
			s_ButtonsdownLastFrame[i] = s_Buttonsdown[i];
			s_Buttonsdown[i] = sf::Mouse::isButtonPressed((sf::Mouse::Button)i);
		}
	}

	void Mouse::Clear()
	{
		for (size_t i = 0; i < sf::Mouse::ButtonCount; ++i)
		{
			s_ButtonsdownLastFrame[i] = false;
			s_Buttonsdown[i] = false;
		}
	}

	bool Mouse::IsButtonDown(sf::Mouse::Button button)
	{
		return s_Buttonsdown[button];
	}

	bool Mouse::IsButtonPressed(sf::Mouse::Button button)
	{
		return s_Buttonsdown[button] && s_ButtonsdownLastFrame[button] == false;
	}

	bool Mouse::MovedLastFrame()
	{
		return s_MousePosLastFrame != sf::Mouse::getPosition();
	}

	void Mouse::SetMousePosLastFrame()
	{
		s_MousePosLastFrame = sf::Mouse::getPosition();
	}

	bool Mouse::IsButtonReleased(sf::Mouse::Button button)
	{
		return s_Buttonsdown[button] == false && s_ButtonsdownLastFrame[button];
	}
} // namespace apex
