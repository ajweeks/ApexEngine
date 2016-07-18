#pragma once

#include <SFML\Window\Event.hpp>

class ApexWindowListener
{
public:
	ApexWindowListener();
	virtual ~ApexWindowListener();

	ApexWindowListener(const ApexWindowListener&) = delete;
	ApexWindowListener& operator=(const ApexWindowListener&) = delete;

	virtual void OnWindowResize(sf::Vector2u windowSize) {};
	virtual void OnWindowFocus() {};
	virtual void OnWindowDefocus() {};

private:

};
