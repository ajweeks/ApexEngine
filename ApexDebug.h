#pragma once

#include <string>
#include <vector>
#include <SFML\Graphics.hpp>
#include "Game.h"
#include "CollapsibleElement.h"

class ApexDebug
{
public:
	ApexDebug();
	virtual ~ApexDebug();

	void Tick(sf::Time elapsed, Game* game);
	void UpdateBackgroundRects();
	void Draw(sf::RenderTarget& target) const;

	CollapsibleElement* AddCollapsibleElementChild(CollapsibleElement* parentElement, const std::string& text);
	CollapsibleElement* CreateCollapsibleElementStack(const std::string& parentText, const sf::Vector2f& parentPosition);

private:
	struct CollapsibleElementStack
	{
		CollapsibleElement* m_CollapsibleElement = nullptr;
		sf::RectangleShape m_BackgroundRect;
	};

	std::vector<CollapsibleElementStack> m_CollapsibleElementStacks;
};
