#pragma once

#include <SFML\System\Time.hpp>
#include <SFML\Window\Mouse.hpp>

namespace apex
{
	class Mouse
	{
	public:
		Mouse();
		virtual ~Mouse();

		static void Tick(sf::Time elapsed);
		static void Clear(); // Forget all inputs
		static bool IsButtonDown(sf::Mouse::Button button);
		static float GetButtonDownTime(sf::Mouse::Button button);
		static bool IsButtonReleased(sf::Mouse::Button button);
		static bool IsButtonPressed(sf::Mouse::Button button);

		static bool MovedLastFrame();
		static sf::Vector2i GetMovementLastFrame();
		static sf::Vector2i GetDraggedVector();
		static sf::Vector2i GetDragStartPos();
		static void SetPosLastFrame();

	private:
		static float s_Buttonsdown[sf::Mouse::ButtonCount];
		static bool s_ButtonsdownLastFrame[sf::Mouse::ButtonCount];
		static sf::Vector2i s_DraggedVector;
		static sf::Vector2i s_PosLastFrame;
		static sf::Vector2i s_DragStart;

	};
} // namespace apex
