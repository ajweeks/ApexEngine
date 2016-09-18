#pragma once

#include <SFML\Window\Event.hpp>

namespace apex
{
	class WindowListener
	{
	public:
		WindowListener();
		virtual ~WindowListener();

		WindowListener(const WindowListener&) = delete;
		WindowListener& operator=(const WindowListener&) = delete;

		virtual void OnWindowResize(sf::Vector2u windowSize) {};
		virtual void OnWindowFocus() {};
		virtual void OnWindowDefocus() {};

	private:

	};
} // namespace apex
