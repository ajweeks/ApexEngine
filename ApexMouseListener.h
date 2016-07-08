#pragma once

#include <SFML\Window\Event.hpp>

class ApexMouseListener
{
public:
	ApexMouseListener();
	virtual ~ApexMouseListener();

	ApexMouseListener(const ApexMouseListener&) = delete;
	ApexMouseListener& operator=(const ApexMouseListener&) = delete;

	// Return whether or not to propogate the event
	// eg. return false if no other listeners should 'hear' this event
	virtual bool OnButtonPress(sf::Event::MouseButtonEvent buttonEvent) = 0;
	virtual void OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent) = 0;
	virtual void OnScroll(sf::Event::MouseWheelScrollEvent scrollEvent) = 0;

private:

};
