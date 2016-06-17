
#include "ApexDebug.h"
#include "Game.h"

ApexDebug::ApexDebug()
{
}

ApexDebug::~ApexDebug()
{
	for (size_t i = 0; i < m_CollapsibleElementStacks.size(); ++i)
	{
		delete m_CollapsibleElementStacks[i].m_CollapsibleElement;
	}
}

void ApexDebug::Tick(sf::Time elapsed, Game* game)
{
	game->SetCursor(sf::StandardCursor::NORMAL);
	for (size_t i = 0; i < m_CollapsibleElementStacks.size(); ++i)
	{
		m_CollapsibleElementStacks[i].m_CollapsibleElement->Tick(elapsed, game, this);
	}
}

void ApexDebug::UpdateBackgroundRects()
{
	for (size_t i = 0; i < m_CollapsibleElementStacks.size(); ++i)
	{
		CollapsibleElement* parentElement = m_CollapsibleElementStacks[i].m_CollapsibleElement;
		const sf::Vector2f pos = parentElement->GetPosition() + sf::Vector2f(-25, -25);
		const float stackHeight = parentElement->GetStackHeight() + 35;
		const float stackWidth = parentElement->GetStackWidth() + 20;
		m_CollapsibleElementStacks[i].m_BackgroundRect = sf::RectangleShape(sf::Vector2f(stackWidth, stackHeight));
		m_CollapsibleElementStacks[i].m_BackgroundRect.setPosition(pos);
		m_CollapsibleElementStacks[i].m_BackgroundRect.setFillColor(sf::Color(50, 55, 60, 135));
	}
}

void ApexDebug::Draw(sf::RenderTarget& target) const
{
	for (size_t i = 0; i < m_CollapsibleElementStacks.size(); ++i)
	{
		target.draw(m_CollapsibleElementStacks[i].m_BackgroundRect);

		m_CollapsibleElementStacks[i].m_CollapsibleElement->Draw(target);
	}
}

CollapsibleElement* ApexDebug::CreateCollapsibleElementStack(const std::string& parentText, const sf::Vector2f& parentPosition)
{
	CollapsibleElement* newElement = new CollapsibleElement(parentText, nullptr);
	newElement->Move(parentPosition);
	CollapsibleElementStack newStack;
	newStack.m_CollapsibleElement = newElement;
	m_CollapsibleElementStacks.push_back(newStack);

	UpdateBackgroundRects();
	return newElement;
}

CollapsibleElement* ApexDebug::AddCollapsibleElementChild(CollapsibleElement* parentElement, const std::string& text)
{
	CollapsibleElement* newElement = parentElement->AddChildElement(new CollapsibleElement(text, parentElement));

	UpdateBackgroundRects();
	return newElement;
}
