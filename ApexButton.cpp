
#include "ApexButton.h"
#include "ApexMain.h"
#include "ApexMouse.h"

ApexButton::ApexButton(float centerX, float centerY, sf::Vector2f size, std::string text) :
	ApexMouseListener()
{
	m_BoundingRect = sf::RectangleShape(size);
	m_BoundingRect.setPosition(centerX - size.x / 2, centerY - size.y / 2);
	m_FillColour = sf::Color(150, 220, 110);
	m_HoverFillColour = sf::Color(100, 180, 90);
	m_BoundingRect.setFillColor(m_FillColour);

	m_Text = sf::Text(text, APEX->FontOpenSans);
	m_Text.setCharacterSize(48);
	m_Text.setStyle(sf::Text::Bold);
	m_Text.setPosition(centerX - 25 * text.length() / 2, centerY - 24);
	m_TextColour = sf::Color(80, 15, 45);
	m_Text.setColor(m_TextColour);
}

ApexButton::~ApexButton()
{
}

void ApexButton::Tick(sf::Time elapsed)
{
	sf::FloatRect rect = m_BoundingRect.getGlobalBounds();

	const bool wasHovering = m_Hovering;
	m_Hovering = rect.contains(static_cast<sf::Vector2f>(APEX->GetMouseCoordsScreenSpace()));
	if (wasHovering != m_Hovering)
	{
		m_BoundingRect.setFillColor(m_Hovering ? m_HoverFillColour : m_FillColour);
	}
}

void ApexButton::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	target.draw(m_BoundingRect, states);
	target.draw(m_Text, states);
}

void ApexButton::SetFillColour(sf::Color fillColour)
{
	m_BoundingRect.setFillColor(fillColour);
}

void ApexButton::SetTextColour(sf::Color textColour)
{
	m_Text.setColor(textColour);
}

bool ApexButton::IsDown() const
{
	return m_IsDown;
}

bool ApexButton::OnButtonPress(sf::Event::MouseButtonEvent buttonEvent)
{
	if (m_Hovering && buttonEvent.button == sf::Mouse::Button::Left)
	{
		m_IsDown = true;
		return false;
	}
	return true;
}

void ApexButton::OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent)
{
	m_IsDown = false;
}

void ApexButton::OnScroll(sf::Event::MouseWheelScrollEvent scrollEvent)
{
}
