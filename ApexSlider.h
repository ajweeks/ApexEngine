#pragma once

#include "ApexMouseListener.h"

#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RectangleShape.hpp>

class ApexSlider : public ApexMouseListener
{
public:
	ApexSlider(float width, float height, float left, float top, float initalValue = 0.5f);
	virtual ~ApexSlider();

	ApexSlider(const ApexSlider&) = delete;
	ApexSlider& operator=(const ApexSlider&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

	float GetValue() const;

	virtual bool OnButtonPress(sf::Event::MouseButtonEvent buttonEvent);
	virtual void OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent);
	virtual void OnScroll(sf::Event::MouseWheelScrollEvent scrollEvent) {};

private:
	sf::RectangleShape m_BackgroundRectangle;
	sf::RectangleShape m_GripRectangle;
	sf::Color m_BackgroundColor;
	sf::Color m_BackgroundColorHover;
	sf::Color m_GripColor;
	sf::Color m_GripColorDragged;

	bool m_BeingDragged = false;
	bool m_HoveredOver = false;

};
