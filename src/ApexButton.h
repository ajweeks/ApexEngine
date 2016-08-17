#pragma once

#include "ApexMouseListener.h"

#include <SFML\Graphics\RectangleShape.hpp>
#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\Text.hpp>

class ApexButton : public ApexMouseListener
{
public:
	static const sf::Color FONT_COLOR;
	static const sf::Color BG_COLOR;
	static const sf::Color BG_HOVER_COLOR;

	ApexButton(float left, float top, float width, float height, std::string text, unsigned int characterSize = 48);
	virtual ~ApexButton();

	ApexButton(const ApexButton&) = delete;
	ApexButton& operator=(const ApexButton&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

	void SetFillColour(sf::Color fillColour);
	void SetTextColour(sf::Color textColour);

	bool IsDown() const;
	bool IsPressed() const;
	void SetString(size_t index, sf::String string);
	void AddString(sf::String string);
	sf::String GetCurrentString() const;
	sf::Vector2f GetPosition() const;

	void SetShowingBackground(bool show);

	void ClearInputs();
	sf::RectangleShape GetRectangle() const;

	bool IsHovering() const;
	
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

	bool m_ShowBackground = true;
};
