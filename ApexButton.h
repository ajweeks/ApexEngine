#pragma once

#include "ApexMouseListener.h"
#include <SFML\Graphics\RectangleShape.hpp>
#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\RenderStates.hpp>
#include <SFML\Graphics\Text.hpp>

class ApexButton : public ApexMouseListener
{
public:
	ApexButton(float centerX, float centerY, sf::Vector2f size, std::string text);
	virtual ~ApexButton();

	ApexButton(const ApexButton&) = delete;
	ApexButton& operator=(const ApexButton&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

	void SetFillColour(sf::Color fillColour);
	void SetTextColour(sf::Color textColour);

	bool IsDown() const;
	
	virtual bool OnButtonPress(sf::Event::MouseButtonEvent buttonEvent);
	virtual void OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent);
	virtual void OnScroll(sf::Event::MouseWheelScrollEvent scrollEvent);

private:
	sf::RectangleShape m_BoundingRect;
	sf::Color m_FillColour;
	sf::Color m_HoverFillColour;
	sf::Color m_TextColour;
	sf::Text m_Text;

	bool m_Hovering = false;
	bool m_IsDown = false;

};
