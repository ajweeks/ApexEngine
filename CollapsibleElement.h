#pragma once

#include <SFML\Graphics.hpp>
#include "Game.h"

class CollapsibleElement
{
public:
	CollapsibleElement(std::string string, CollapsibleElement* parent, bool collapsed = false);
	virtual ~CollapsibleElement();

	void Tick(sf::Time elapsed, Game* game, ApexDebug* debug);
	void Draw(sf::RenderTarget& target);
	void SetCollapsed(bool collapsed);
	void Move(const sf::Vector2f& deltaPosition);
	sf::Vector2f GetPosition() const;
	float GetStackWidth(); // Returns the height of the stack of all child elements (at minimum will return LINE_HEIGHT)
	float GetStackHeight(); // Returns the height of the stack of all child elements (at minimum will return LINE_HEIGHT)


private:
	static const float INDENTATION;
	static const float LINE_HEIGHT;
	static const int FONT_SIZE;

	CollapsibleElement* AddChildElement(CollapsibleElement* newElement);
	void DrawTriangle(sf::RenderTarget& target, const sf::Vector2f centerPos, bool rotatedDown) const;
	sf::FloatRect GetBounds(sf::Text text);
	void UpdateChildrenPositions();
	float GetFarthestRightPointInStack(float farthestSoFar);

	sf::Vector2f m_Position;
	sf::Text m_Text;
	CollapsibleElement* m_Parent = nullptr;
	std::vector<CollapsibleElement*> m_Children;
	bool m_Collapsed = true;
	bool m_Hover = false; // True when the mouse is hovering over this element (text or arrow)

	friend class ApexDebug;
};
