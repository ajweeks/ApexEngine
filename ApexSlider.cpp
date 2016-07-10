
#include "ApexSlider.h"
#include "ApexMain.h"

ApexSlider::ApexSlider(float width, float height, float left, float top, float initalValue)
{
	m_BackgroundRectangle = sf::RectangleShape(sf::Vector2f(width, height));
	m_BackgroundRectangle.setPosition(left, top);

	m_GripRectangle = sf::RectangleShape(sf::Vector2f(width / 15.0f, height * 1.2f));
	m_GripRectangle.setPosition(left + initalValue * width, top - 0.1f * height);

	m_BackgroundColor = sf::Color(90, 50, 45);
	m_BackgroundColorHover = sf::Color(110, 70, 65);
	m_BackgroundRectangle.setFillColor(m_BackgroundColor);
	m_BackgroundRectangle.setOutlineColor(sf::Color::Black);
	m_BackgroundRectangle.setOutlineThickness(2.0f);

	m_GripColor = sf::Color(195, 180, 180);
	m_GripColorDragged = sf::Color(250, 240, 240);
	m_GripRectangle.setFillColor(m_GripColor);
	m_GripRectangle.setOutlineColor(sf::Color::Black);
	m_GripRectangle.setOutlineThickness(1.0f);
}

ApexSlider::~ApexSlider()
{
}

void ApexSlider::Tick(sf::Time elapsed)
{
	const bool wasHoveredOver = m_HoveredOver;
	const sf::Vector2i mousePosScreenSpace = APEX->GetMouseCoordsScreenSpace();
	m_HoveredOver = m_BackgroundRectangle.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosScreenSpace));
	if (wasHoveredOver != m_HoveredOver)
	{
		m_BackgroundRectangle.setFillColor(m_HoveredOver ? m_BackgroundColorHover : m_BackgroundColor);
	}

	if (m_BeingDragged)
	{
		const sf::Vector2f bgRectPos = m_BackgroundRectangle.getPosition();
		const sf::Vector2f bgRectSize = m_BackgroundRectangle.getSize();
		const sf::Vector2f gripRectPos = m_GripRectangle.getPosition();
		const sf::Vector2f gripRectSize = m_GripRectangle.getSize();
		
		if (mousePosScreenSpace.x - gripRectSize.x / 2.0f < bgRectPos.x)
		{
			m_GripRectangle.setPosition(bgRectPos.x, gripRectPos.y);
			return;
		}
		else if (mousePosScreenSpace.x + gripRectSize.x / 2.0f > bgRectPos.x + bgRectSize.x)
		{ 
			m_GripRectangle.setPosition(bgRectPos.x + bgRectSize.x - gripRectSize.x, gripRectPos.y);
			return;
		}

		const float dx = (mousePosScreenSpace.x - bgRectPos.x) / bgRectSize.x;
		m_GripRectangle.setPosition(bgRectPos.x - gripRectSize.x / 2.0f + dx * bgRectSize.x, gripRectPos.y);
	}
}

void ApexSlider::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	target.draw(m_BackgroundRectangle, states);
	target.draw(m_GripRectangle, states);
}

float ApexSlider::GetValue() const
{
	return (m_GripRectangle.getPosition().x - m_BackgroundRectangle.getPosition().x) / m_BackgroundRectangle.getSize().x;
}

bool ApexSlider::OnButtonPress(sf::Event::MouseButtonEvent buttonEvent)
{
	if (m_HoveredOver && buttonEvent.button == sf::Mouse::Button::Left)
	{
		m_BeingDragged = true;
		m_GripRectangle.setFillColor(m_GripColorDragged);
	}
	return true;
}

void ApexSlider::OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent)
{
	m_BeingDragged = false;
	m_GripRectangle.setFillColor(m_GripColor);
}
