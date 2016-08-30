
#include "ApexSlider.h"
#include "ApexMain.h"

const float ApexSlider::GRIP_MOVE_MS_PER_PIXEL = 8.5f;
const sf::Int32 ApexSlider::GRIP_COLOR_TRANSITION_MS = 90;
const ApexTransition::EaseType ApexSlider::GRIP_MOVE_EASE_TYPE = ApexTransition::EaseType::CIRCULAR_OUT;
const float ApexSlider::GRIP_REL_HEIGHT = 1.4f; // The grip is 1.4 times taller than the bg rect
const float ApexSlider::GRIP_REL_WIDTH = 1.0f / 15.0f; // The grip is 15 times less wide than the bg rect

ApexSlider::ApexSlider(float width, float height, float left, float top, float initalValue)
{
	m_BackgroundRectangle = sf::RectangleShape(sf::Vector2f(width, height));
	m_BackgroundRectangle.setPosition(left, top);

	m_BackgroundColor = sf::Color(50, 110, 130);
	const int hoverDelta = -20;
	m_BackgroundColorHover = sf::Color(m_BackgroundColor.r + hoverDelta, m_BackgroundColor.g + hoverDelta, m_BackgroundColor.b + hoverDelta);
	m_BackgroundRectangle.setFillColor(m_BackgroundColor);
	m_BackgroundRectangle.setOutlineColor(sf::Color::Black);
	m_BackgroundRectangle.setOutlineThickness(2.0f);

	CreateGripTransformableTransition(left + initalValue * width, top - ((GRIP_REL_HEIGHT-1.0f) / 2.0f) * height, true);

	m_GripRectangle = sf::RectangleShape(sf::Vector2f(width / 15.0f, height * GRIP_REL_HEIGHT));
	m_GripRectangle.setPosition(m_GripTransformableTransition.GetCurrentTransformable().getPosition());
	m_GripColor = sf::Color(195, 180, 185);
	m_GripColorDragged = sf::Color(250, 240, 245);
	m_GripColorTransition = ColorTransition(m_GripColorDragged, m_GripColor, sf::milliseconds(GRIP_COLOR_TRANSITION_MS));
	m_GripColorTransition.SetFinished();
	m_GripRectangle.setFillColor(m_GripColorTransition.GetCurrentColor());
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
		const sf::Vector2f bgPos = m_BackgroundRectangle.getPosition();
		const sf::Vector2f bgSize = m_BackgroundRectangle.getSize();
		const sf::Vector2f gripPos = m_GripRectangle.getPosition();
		const sf::Vector2f gripSize = m_GripRectangle.getSize();
		
		if (mousePosScreenSpace.x - gripSize.x / 2.0f < bgPos.x)
		{
			CreateGripTransformableTransition(bgPos.x, gripPos.y);
		}
		else if (mousePosScreenSpace.x + gripSize.x / 2.0f > bgPos.x + bgSize.x)
		{ 
			CreateGripTransformableTransition(bgPos.x + bgSize.x - gripSize.x, gripPos.y);
		}
		else
		{
			const float dx = (mousePosScreenSpace.x - bgPos.x) / bgSize.x;
			CreateGripTransformableTransition(bgPos.x - gripSize.x / 2.0f + dx * bgSize.x, gripPos.y);
		}
	}

	m_GripColorTransition.Tick(elapsed);
	m_GripRectangle.setFillColor(m_GripColorTransition.GetCurrentColor());

	m_GripTransformableTransition.Tick(elapsed);
	m_GripRectangle.setPosition(m_GripTransformableTransition.GetCurrentTransformable().getPosition());
}

void ApexSlider::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	target.draw(m_BackgroundRectangle, states);
	target.draw(m_GripRectangle, states);
}

void ApexSlider::CreateGripTransformableTransition(sf::Vector2f endPos, bool instant)
{
	CreateGripTransformableTransition(endPos.x, endPos.y, instant);
}

void ApexSlider::CreateGripTransformableTransition(float endPosX, float endPosY, bool instant)
{
	const sf::Vector2f gripPos = m_GripRectangle.getPosition();
	const float dx = gripPos.x - endPosX;
	
	if (abs(dx) >= 1.0f)
	{
		sf::Transformable start;
		start.setPosition(gripPos);
		sf::Transformable end;
		end.setPosition(endPosX, endPosY);
		const sf::Time duration = sf::milliseconds(sf::Int32(abs(dx) * GRIP_MOVE_MS_PER_PIXEL));
		if (!instant && duration.asMilliseconds() > 10)
		{
			m_GripTransformableTransition = TransformableTransition(start, end, duration, GRIP_MOVE_EASE_TYPE);
			m_GripTransformableTransition.Restart();
		}
		else
		{
			m_GripTransformableTransition = TransformableTransition(start, end, sf::milliseconds(1), GRIP_MOVE_EASE_TYPE);
			m_GripTransformableTransition.SetFinished();
		}
	}
}

void ApexSlider::SetValue(float value)
{
	assert(value >= 0.0f && value <= 1.0f);
	const float bgWidth = m_BackgroundRectangle.getSize().x;
	const float bgLeft = m_BackgroundRectangle.getPosition().x;
	CreateGripTransformableTransition(bgLeft + value * (bgWidth), m_GripRectangle.getPosition().y, true);
	m_GripRectangle.setPosition(m_GripTransformableTransition.GetCurrentTransformable().getPosition());
}

float ApexSlider::GetValue() const
{
	const float maxWidth = m_BackgroundRectangle.getSize().x - m_GripRectangle.getSize().x;
	const float gripLeft = m_GripTransformableTransition.GetEndTransformable().getPosition().x;
	const float bgLeft = m_BackgroundRectangle.getPosition().x;
	return (gripLeft - bgLeft) / maxWidth;
}

bool ApexSlider::BeingDragged() const
{
	return m_BeingDragged;
}

bool ApexSlider::OnButtonPress(sf::Event::MouseButtonEvent buttonEvent)
{
	if (m_HoveredOver && buttonEvent.button == sf::Mouse::Button::Left)
	{
		m_BeingDragged = true;
		m_GripColorTransition.SwapAndRestart();
	}
	return true;
}

void ApexSlider::OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent)
{
	if (m_BeingDragged)
	{
		m_GripColorTransition.SwapAndRestart();
		m_BeingDragged = false;
	}
}
