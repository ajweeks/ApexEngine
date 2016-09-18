#pragma once

#include <SFML\Window\Event.hpp>

namespace apex
{
	class MouseListener
	{
	public:
		MouseListener();
		virtual ~MouseListener();

		MouseListener(const MouseListener&) = delete;
		MouseListener& operator=(const MouseListener&) = delete;

		// Return whether or not to propogate the event
		// eg. return false if no other listeners should 'hear' this event
		virtual bool OnButtonPress(sf::Event::MouseButtonEvent buttonEvent) = 0;
		virtual void OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent) = 0;
		virtual void OnScroll(sf::Event::MouseWheelScrollEvent scrollEvent) = 0;

	private:

	};
} // namespace apex
