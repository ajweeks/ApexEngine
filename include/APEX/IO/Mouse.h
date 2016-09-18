#pragma once

#include <SFML\Window\Mouse.hpp>

namespace apex
{
	class Mouse
	{
	public:
		Mouse();
		virtual ~Mouse();

		static void Tick();
		static void Clear(); // Forget all inputs
		static bool IsButtonDown(sf::Mouse::Button button);
		static bool IsButtonReleased(sf::Mouse::Button button);
		static bool IsButtonPressed(sf::Mouse::Button button);
		static bool MovedLastFrame();
		static void SetMousePosLastFrame();

	private:
		static bool s_Buttonsdown[sf::Mouse::ButtonCount];
		static bool s_ButtonsdownLastFrame[sf::Mouse::ButtonCount];
		static bool s_MouseMovedLastFrame;
		static sf::Vector2i s_MousePosLastFrame;
	};
} // namespace apex
