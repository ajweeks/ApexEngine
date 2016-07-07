#pragma once

#include <SFML\Window\Event.hpp>

class ApexMouseListener
{
public:
	ApexMouseListener();
	virtual ~ApexMouseListener();

	ApexMouseListener(const ApexMouseListener&) = delete;
	ApexMouseListener& operator=(const ApexMouseListener&) = delete;

	virtual void OnButtonPress(sf::Event::MouseButtonEvent buttonEvent) = 0;
	virtual void OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent) = 0;
	virtual void OnScroll(sf::Event::MouseWheelScrollEvent scrollEvent) = 0;

private:

};
