
#include "Apex\IO\Mouse.h"

namespace apex
{
	float Mouse::s_Buttonsdown[sf::Mouse::ButtonCount];
	bool Mouse::s_ButtonsdownLastFrame[sf::Mouse::ButtonCount];
	sf::Vector2i Mouse::s_DraggedVector;
	sf::Vector2i Mouse::s_PosLastFrame;
	sf::Vector2i Mouse::s_DragStart = sf::Vector2i(-1, -1);

	Mouse::Mouse()
	{
	}

	Mouse::~Mouse()
	{
	}

	void Mouse::Tick(sf::Time elapsed)
	{
		const float elapsedS = elapsed.asSeconds();
		for (size_t i = 0; i < sf::Mouse::ButtonCount; ++i)
		{
			const bool wasButtonDown = (s_Buttonsdown[i] >= 0.0f);
			const bool isButtonDown = sf::Mouse::isButtonPressed(sf::Mouse::Button(i));
			s_ButtonsdownLastFrame[i] = wasButtonDown;

			if (isButtonDown)
			{
				if (!wasButtonDown) 
				{
					s_Buttonsdown[i] = 0.0f;
					if (i == int(sf::Mouse::Button::Left))
					{
						s_DragStart = sf::Mouse::getPosition();
					}
				}
				else 
				{
					s_Buttonsdown[i] += elapsedS;
				}
			}
			else
			{
				s_Buttonsdown[i] = -1.0f;
				if (i == int(sf::Mouse::Button::Left))
				{
					s_DragStart = sf::Vector2i(-1, -1);
					s_DraggedVector = sf::Vector2i(0, 0);
				}
			}
		}

		if (s_DragStart != sf::Vector2i(-1, -1))
		{
			s_DraggedVector = sf::Mouse::getPosition() - s_DragStart;
		}
	}

	void Mouse::Clear()
	{
		for (size_t i = 0; i < sf::Mouse::ButtonCount; ++i)
		{
			s_ButtonsdownLastFrame[i] = false;
			s_Buttonsdown[i] = -1.0f;
		}
		s_DragStart = sf::Vector2i(-1, -1);
		s_DraggedVector = sf::Vector2i(0, 0);
	}

	bool Mouse::IsButtonDown(sf::Mouse::Button button)
	{
		return (s_Buttonsdown[button] >= 0.0f);
	}

	float Mouse::GetButtonDownTime(sf::Mouse::Button button)
	{
		return s_Buttonsdown[button];
	}

	bool Mouse::IsButtonPressed(sf::Mouse::Button button)
	{
		return (s_Buttonsdown[button] == 0.0f) && s_ButtonsdownLastFrame[button] == false;
	}

	bool Mouse::IsButtonReleased(sf::Mouse::Button button)
	{
		return (s_Buttonsdown[button] < 0.0f) && s_ButtonsdownLastFrame[button];
	}

	bool Mouse::MovedLastFrame()
	{
		return s_PosLastFrame != sf::Mouse::getPosition();
	}

	sf::Vector2i Mouse::GetDraggedVector()
	{
		return s_DraggedVector;
	}

	sf::Vector2i Mouse::GetDragStartPos()
	{
		return s_DragStart;
	}

	void Mouse::SetPosLastFrame()
	{
		s_PosLastFrame = sf::Mouse::getPosition();
	}

	sf::Vector2i Mouse::GetMovementLastFrame()
	{
		const sf::Vector2i movement = sf::Mouse::getPosition() - s_PosLastFrame;
		return 	movement;
	}
} // namespace apex
