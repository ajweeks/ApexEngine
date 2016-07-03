#pragma once

#include "CollapsibleElement.h"
#include <string>
#include <vector>
#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\RectangleShape.hpp>

class ApexDebug
{
public:
	ApexDebug();
	virtual ~ApexDebug();

	ApexDebug(const ApexDebug&) = delete;
	ApexDebug& operator=(const ApexDebug&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	struct CollapsibleElementStack
	{
		CollapsibleElement* m_CollapsibleElement = nullptr;
		sf::RectangleShape m_BackgroundRect;
	};

	CollapsibleElement* AddCollapsibleElementChild(CollapsibleElement* parentElement, const std::string& string);
	CollapsibleElementStack* CreateCollapsibleElementStack(const std::string& string, const sf::Vector2f& position);
	void UpdateBackgroundRect(CollapsibleElementStack* stack);

	// Only tick and paint all "parent" elements (stacks), which in turn paint and tick their children
	CollapsibleElementStack* m_PlayerElementStack;
	CollapsibleElement* m_PlayerPosElement;
	CollapsibleElement* m_PlayerVelElement;
};
