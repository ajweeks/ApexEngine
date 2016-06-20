#pragma once

#include <SFML\Graphics.hpp>
#include "Game.h"

class CollapsibleElement
{
public:
	// Use this constructor for elements which need to call the passed in callback every frame to update their value
	CollapsibleElement(CollapsibleElement* parent, std::string prefixString, float (*GetValue)(), float initalValue = 0.0f, bool collapsed = false);
	// Use this constructor for elements which only display a non-changing string
	CollapsibleElement(CollapsibleElement* parent, std::string string, bool collapsed = false);
	virtual ~CollapsibleElement();

	CollapsibleElement(const CollapsibleElement&) = delete;
	CollapsibleElement& operator=(const CollapsibleElement&) = delete;

	// Returns true if the width of the stack this element is in needs resizing
	bool Tick(sf::Time elapsed, Game* game, ApexDebug* debug);
	void Draw(sf::RenderTarget& target);
	void SetCollapsed(bool collapsed);
	void Move(const sf::Vector2f& deltaPosition);
	sf::Vector2f GetPosition() const;
	float GetStackWidth(); // Returns the height of the stack of all child elements (at minimum will return LINE_HEIGHT)
	float GetStackHeight(); // Returns the height of the stack of all child elements (at minimum will return LINE_HEIGHT)

	void UpdateString(std::string newString, bool usePrefix = true);

private:
	static const float INDENTATION;
	static const float LINE_HEIGHT;
	static const int FONT_SIZE;

	CollapsibleElement* AddChildElement(CollapsibleElement* newElement);
	void DrawTriangle(sf::RenderTarget& target, const sf::Vector2f centerPos, bool rotatedDown) const;

	sf::FloatRect GetBounds(sf::Text text);
	float GetFarthestRightPointInStack(float farthestSoFar);
	void UpdateChildrenPositions();

	sf::Vector2f m_Position;
	std::string m_PrefixString;
	sf::Text m_Text;
	CollapsibleElement* m_Parent = nullptr;
	std::vector<CollapsibleElement*> m_Children;
	bool m_Collapsed = true;
	bool m_Hover = false; // True when the mouse is hovering over this element (text or arrow)

	// A function callback which get's called every frame to update this element's value
	float (*GetValue)() = nullptr;
	float m_Value = 0.0f;

	friend class ApexDebug;
};
