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
	ApexButton(float left, float top, float width, float height, std::string text);
	virtual ~ApexButton();

	ApexButton(const ApexButton&) = delete;
	ApexButton& operator=(const ApexButton&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

	void SetFillColour(sf::Color fillColour);
	void SetTextColour(sf::Color textColour);

	bool IsDown() const;
	bool IsPressed() const;
	void AddString(sf::String string);
	sf::String GetCurrentString() const;

	void ClearInputs();
	
	virtual bool OnButtonPress(sf::Event::MouseButtonEvent buttonEvent);
	virtual void OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent);
	virtual void OnScroll(sf::Event::MouseWheelScrollEvent scrollEvent);

private:
	void SetHovering(bool hovering);

	sf::RectangleShape m_BoundingRect;
	sf::Color m_FillColour;
	sf::Color m_HoverFillColour;
	sf::Color m_TextColour;
	sf::Text m_Text;
	std::vector<sf::String> m_StringOptions; // Typically just one string, but can hold multiple and cycle through on click
	int m_CurrentStringIndex;

	bool m_Hovering = false;
	bool m_IsDown = false;
	bool m_IsPressed = false;
};
