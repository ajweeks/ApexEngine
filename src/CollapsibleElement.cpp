
#include "CollapsibleElement.h"
#include "ApexMain.h"
#include "ApexMouse.h"
#include "ApexDebug.h"

#include <SFML\Graphics\CircleShape.hpp>

const float CollapsibleElement::INDENTATION = 35.0f;
const float CollapsibleElement::LINE_HEIGHT = 50.0f;
const int CollapsibleElement::FONT_SIZE = 32;

CollapsibleElement::CollapsibleElement(CollapsibleElement* parent, std::string string, bool collapsed) :
	m_PrefixString(string), m_Text(string, ApexMain::FontOpenSans, FONT_SIZE), m_Parent(parent), m_Collapsed(collapsed)
{
	m_Text.move(18, 0);
}

CollapsibleElement::~CollapsibleElement()
{
	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		delete m_Children[i];
	}
}

bool CollapsibleElement::Tick(sf::Time elapsed, ApexDebug* debug)
{
	bool rectNeedsResizing = m_NeedsBackgroundResize;
	sf::Vector2i mousePos = APEX->GetMouseCoordsScreenSpace();

	sf::FloatRect globalBounds = GetBounds(m_Text);

	const bool wasHovering = m_Hover;
	m_Hover = (globalBounds.contains(sf::Vector2f(mousePos)));
	if (m_Hover) APEX->SetCursor(ApexCursor::POINT);

	if (m_Collapsed == false)
	{
		for (size_t i = 0; i < m_Children.size(); ++i)
		{
			if (m_Children[i]->Tick(elapsed, debug)) rectNeedsResizing = true;
		}
	}

	m_NeedsBackgroundResize = false;
	return rectNeedsResizing;
}

void CollapsibleElement::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	if (m_Children.empty() == false)
	{
		DrawTriangle(target, m_Position + sf::Vector2f(0, 20), m_Collapsed, states);
	}

	if (m_Hover) 
	{
		m_Text.setColor(sf::Color(250, 250, 250));
		m_Text.setStyle(sf::Text::Bold);
	}
	else 
	{
		m_Text.setColor(sf::Color(200, 200, 215));
	}
	target.draw(m_Text, states);

	if (!m_Collapsed)
	{
		for (size_t i = 0; i < m_Children.size(); ++i)
		{
			m_Children[i]->Draw(target, states);
		}
	}
}

void CollapsibleElement::DrawTriangle(sf::RenderTarget& target, const sf::Vector2f centerPos, bool rotatedDown, sf::RenderStates states) const
{
	static const float HALF_TRIANGLE_WIDTH = 15.0f;
	sf::CircleShape triangle(float(HALF_TRIANGLE_WIDTH), 3);
	triangle.setPosition(centerPos);
	triangle.setOrigin(sf::Vector2f(HALF_TRIANGLE_WIDTH, HALF_TRIANGLE_WIDTH));
	const float rotation = rotatedDown ? 90.0f : 180.0f;
	triangle.setRotation(rotation);
	triangle.setFillColor(sf::Color(190, 190, 200));
	target.draw(triangle, states);
}

CollapsibleElement* CollapsibleElement::AddChildElement(CollapsibleElement* newElement)
{
	const float deltaYPos = GetStackHeight();
	newElement->m_Position = m_Position + sf::Vector2f(float(INDENTATION), deltaYPos);
	newElement->m_Text.move(newElement->m_Position);
	m_Children.push_back(newElement);
	return newElement;
}

sf::FloatRect CollapsibleElement::GetBounds(sf::Text text)
{
	const sf::FloatRect localBounds = text.getLocalBounds();
	const sf::Vector2f size = sf::Vector2f(localBounds.width + 45, localBounds.height + 5);
	return sf::FloatRect(m_Position + sf::Vector2f(-24, 5), size);
}

float CollapsibleElement::GetStackWidth()
{
	const float farthestRight = GetFarthestRightPointInStack(m_Position.x);

	const sf::FloatRect ourBounds = GetBounds(m_Text);
	return farthestRight - ourBounds.left;
}

float CollapsibleElement::GetFarthestRightPointInStack(float farthestRightSoFar)
{
	const sf::FloatRect ourBounds = GetBounds(m_Text);
	const float ourRight = ourBounds.left + ourBounds.width;
	if (ourRight > farthestRightSoFar) farthestRightSoFar = ourRight;

	if (m_Collapsed) return farthestRightSoFar;

	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		const float childsRight = m_Children[i]->GetFarthestRightPointInStack(farthestRightSoFar);
		if (childsRight > farthestRightSoFar)
		{
			farthestRightSoFar = childsRight;
		}
	}

	return farthestRightSoFar;
}

float CollapsibleElement::GetStackHeight()
{
	if (m_Collapsed || m_Children.empty()) return LINE_HEIGHT;

	float stackHeight = LINE_HEIGHT;
	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		stackHeight += m_Children[i]->GetStackHeight();
	}

	return stackHeight;
}

void CollapsibleElement::UpdateString(std::string newString, bool usePrefix)
{
	sf::String str;
	if (usePrefix) str = m_PrefixString + ": " + newString;
	else str = newString;

	m_Text.setString(str);
}

int CollapsibleElement::GetCurrentStringIndex() const
{
	return m_CurrentTextShowingIndex;
}

void CollapsibleElement::SetNumberOfTextValues(int num)
{
	m_NumberOfTextValues = num;
}

bool CollapsibleElement::OnButtonPress(sf::Event::MouseButtonEvent buttonEvent)
{
	if (buttonEvent.button == sf::Mouse::Button::Left && m_Hover)
	{
		if (!m_Children.empty())
		{
			SetCollapsed(!m_Collapsed);
			m_NeedsBackgroundResize = true;
		}
		++m_CurrentTextShowingIndex;
		m_CurrentTextShowingIndex %= m_NumberOfTextValues;
		return false;
	}
	return true;
}

void CollapsibleElement::OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent)
{
}

void CollapsibleElement::OnScroll(sf::Event::MouseWheelScrollEvent scrollEvent)
{
}

void CollapsibleElement::ClearAllInput()
{
	m_Hover = false;
	for (size_t i = 0; i < m_Children.size(); i++)
	{
		m_Children[i]->ClearAllInput();
	}
}

void CollapsibleElement::SetCollapsed(bool collapsed)
{
	if (collapsed == m_Collapsed) return;

	m_Collapsed = collapsed;
	if (m_Parent != nullptr) m_Parent->UpdateChildrenPositions();
}

void CollapsibleElement::UpdateChildrenPositions()
{
	float yPos = m_Position.y + LINE_HEIGHT;
	for (size_t i = 0; i < m_Children.size(); i++)
	{
		m_Children[i]->Move(sf::Vector2f(0, yPos - m_Children[i]->m_Position.y));
		yPos += m_Children[i]->GetStackHeight();
	}
	if (m_Parent != nullptr) m_Parent->UpdateChildrenPositions();
}

void CollapsibleElement::Move(const sf::Vector2f& deltaPosition)
{
	m_Position += deltaPosition;
	m_Text.move(deltaPosition);
	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->Move(deltaPosition);
	}
}

sf::Vector2f CollapsibleElement::GetPosition() const
{
	return m_Position;
}
