
#include "Apex\Graphics\Button.h"
#include "Apex\ApexMain.h"
#include "Apex\IO\Mouse.h"

#include <SFML\Graphics\Shader.hpp>

namespace apex
{
	const sf::Color Button::FONT_COLOR = sf::Color(233, 242, 243);
	const sf::Color Button::BG_COLOR = sf::Color(60, 100, 115);
	const sf::Color Button::BG_HOVER_COLOR = sf::Color(BG_COLOR.r - 20, BG_COLOR.g - 20, BG_COLOR.b - 20);

	Button::Button(float left, float top, float width, float height, std::string text, const sf::Font& font, unsigned int characterSize) :
		MouseListener()
	{
		m_BoundingRect = sf::RectangleShape(sf::Vector2f(width, height));
		m_BoundingRect.setPosition(left, top);
		m_FillColour = BG_COLOR;
		m_HoverFillColour = BG_HOVER_COLOR;
		m_BoundingRect.setFillColor(m_FillColour);

		m_StringOptions.push_back(text);
		m_CurrentStringIndex = 0;
		m_Text = sf::Text(m_StringOptions[m_CurrentStringIndex], font, characterSize);
		m_Text.setStyle(sf::Text::Bold);
		m_Text.setPosition(left + width / 2.0f - 25 * text.length() / 2.0f, top + height / 2.0f - 24.0f);
		m_TextColour = FONT_COLOR;
		m_Text.setFillColor(m_TextColour);
	}

	Button::~Button()
	{
	}

	void Button::Tick(sf::Time elapsed)
	{
		sf::FloatRect rect = m_BoundingRect.getGlobalBounds();
		const sf::Vector2i mousePos = APEX->GetMouseCoordsScreenSpace();
		SetHovering(rect.contains(static_cast<sf::Vector2f>(mousePos)));
	}

	void Button::Draw(sf::RenderTarget& target, sf::RenderStates states)
	{
		if (m_Hovering && m_ShowBackground)
		{
			states.transform.translate(0, 5.0f);
		}

		if (m_ShowBackground)
		{
			target.draw(m_BoundingRect, states);
		}

		// The vignette shader messes up text, don't use it
		states.shader = states.Default.shader;
		target.draw(m_Text, states);
	}

	void Button::SetFillColour(sf::Color fillColour)
	{
		m_BoundingRect.setFillColor(fillColour);
	}

	void Button::SetTextColour(sf::Color textColour)
	{
		m_Text.setFillColor(textColour);
	}

	bool Button::IsDown() const
	{
		return m_IsDown;
	}

	bool Button::IsPressed() const
	{
		return m_IsPressed;
	}

	void Button::SetString(size_t index, sf::String string)
	{
		assert(index >= 0 && index < m_StringOptions.size());
		m_StringOptions[index] = string;
		m_Text.setString(m_StringOptions[index]);
		const sf::Vector2f pos = m_BoundingRect.getPosition();
		const sf::Vector2f size = m_BoundingRect.getSize();
		m_Text.setPosition(pos.x + size.x / 2.0f - 25 * string.getSize() / 2.0f, pos.y + size.y / 2.0f - 24.0f);
	}

	void Button::AddString(sf::String string)
	{
		m_StringOptions.push_back(string);
	}

	sf::String Button::GetCurrentString() const
	{
		return m_Text.getString();
	}

	sf::Vector2f Button::GetPosition() const
	{
		return m_BoundingRect.getPosition();
	}

	void Button::SetShowingBackground(bool show)
	{
		m_ShowBackground = show;
	}

	void Button::SetActive(bool active)
	{
		m_IsDown = active;
		m_IsPressed = active;
		ShowNextString();
	}

	void Button::ClearInputs()
	{
		SetHovering(false);
		m_IsDown = false;
		m_IsPressed = false;
	}

	sf::RectangleShape Button::GetRectangle() const
	{
		return m_BoundingRect;
	}

	void Button::SetHovering(bool hovering)
	{
		const bool wasHovering = m_Hovering;
		if (wasHovering != hovering)
		{
			m_Hovering = hovering;
			m_BoundingRect.setFillColor(m_Hovering ? m_HoverFillColour : m_FillColour);
			//APEX->SetCursor(m_Hovering ? Cursor::POINT : Cursor::NORMAL);
		}
	}

	bool Button::IsHovering() const
	{
		return m_Hovering;
	}

	void Button::SetNeighbor(Button* neighbor, Direction direction)
	{
		m_Neighbors[int(direction)] = neighbor;
	}

	void Button::SetNeighbors(Button* up, Button* down, Button* left, Button* right)
	{
		m_Neighbors[int(Direction::UP)] = up;
		m_Neighbors[int(Direction::DOWN)] = down;
		m_Neighbors[int(Direction::LEFT)] = left;
		m_Neighbors[int(Direction::RIGHT)] = right;
	}

	Button* Button::GetNeighbor(Direction direction)
	{
		return m_Neighbors[int(direction)];
	}

	bool Button::OnButtonPress(sf::Event::MouseButtonEvent buttonEvent)
	{
		if (m_Hovering && buttonEvent.button == sf::Mouse::Button::Left)
		{
			if (!m_IsDown) m_IsPressed = true;
			else m_IsPressed = false;
			m_IsDown = true;

			ShowNextString();

			return true;
		}
		return false;
	}

	void Button::ShowNextString()
	{
		if (m_StringOptions.size() > 1)
		{
			++m_CurrentStringIndex;
			m_CurrentStringIndex %= m_StringOptions.size();
			m_Text.setString(m_StringOptions[m_CurrentStringIndex]);
		}
	}

	void Button::OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent)
	{
		m_IsDown = false;
		m_IsPressed = false;
	}

	void Button::OnScroll(sf::Event::MouseWheelScrollEvent scrollEvent)
	{
	}
} // namespace apex
