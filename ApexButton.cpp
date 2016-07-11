
#include "ApexButton.h"
#include "ApexMain.h"
#include "ApexMouse.h"
#include <SFML\Graphics\Shader.hpp>

ApexButton::ApexButton(float left, float top, float width, float height, std::string text) :
	ApexMouseListener()
{
	m_BoundingRect = sf::RectangleShape(sf::Vector2f(width, height));
	m_BoundingRect.setPosition(left, top);
	m_FillColour = sf::Color(150, 220, 110);
	m_HoverFillColour = sf::Color(100, 180, 90);
	m_BoundingRect.setFillColor(m_FillColour);

	m_StringOptions.push_back(text);
	m_CurrentStringIndex = 0;
	m_Text = sf::Text(m_StringOptions[m_CurrentStringIndex], APEX->FontOpenSans);
	m_Text.setCharacterSize(48);
	m_Text.setStyle(sf::Text::Bold);
	m_Text.setPosition(left + width / 2.0f - 25 * text.length() / 2.0f, top + height / 2.0f - 24.0f);
	m_TextColour = sf::Color(80, 15, 45);
	m_Text.setColor(m_TextColour);
}

ApexButton::~ApexButton()
{
}

void ApexButton::Tick(sf::Time elapsed)
{
	sf::FloatRect rect = m_BoundingRect.getGlobalBounds();
	const sf::Vector2i mousePos = APEX->GetMouseCoordsScreenSpace();
	SetHovering(rect.contains(static_cast<sf::Vector2f>(mousePos)));
}

void ApexButton::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	target.draw(m_BoundingRect, states);

	target.draw(m_Text);
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

bool ApexButton::IsPressed() const
{
	return m_IsPressed;
}

void ApexButton::AddString(sf::String string)
{
	m_StringOptions.push_back(string);
}

sf::String ApexButton::GetCurrentString() const
{
	return m_Text.getString();
}

void ApexButton::ClearInputs()
{
	SetHovering(false);
	m_IsDown = false;
	m_IsPressed = false;
}

sf::RectangleShape ApexButton::GetRectangle() const
{
	return m_BoundingRect;
}

bool ApexButton::OnButtonPress(sf::Event::MouseButtonEvent buttonEvent)
{
	if (m_Hovering && buttonEvent.button == sf::Mouse::Button::Left)
	{
		if (!m_IsDown) m_IsPressed = true;
		else m_IsPressed = false;
		m_IsDown = true;

		if (m_StringOptions.size() > 1)
		{
			++m_CurrentStringIndex;
			m_CurrentStringIndex %= m_StringOptions.size();
			m_Text.setString(m_StringOptions[m_CurrentStringIndex]);
		}

		return false;
	}
	return true;
}

void ApexButton::OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent)
{
	m_IsDown = false;
	m_IsPressed = false;
}

void ApexButton::OnScroll(sf::Event::MouseWheelScrollEvent scrollEvent)
{
}

void ApexButton::SetHovering(bool hovering)
{
	const bool wasHovering = m_Hovering;
	if (wasHovering != hovering)
	{
		m_Hovering = hovering;
		m_BoundingRect.setFillColor(m_Hovering ? m_HoverFillColour : m_FillColour);
		APEX->SetCursor(m_Hovering ? ApexCursor::POINT : ApexCursor::NORMAL);
	}
}
