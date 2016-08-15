#pragma once

#include "TransformableTransition.h"

#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\System\Time.hpp>

#include <map>

class ApexButton;

class ApexButtonList
{
public:
	ApexButtonList();
	virtual ~ApexButtonList();

	ApexButtonList(const ApexButtonList&) = delete;
	ApexButtonList& operator=(const ApexButtonList&) = delete;

	void AddButton(ApexButton* button, size_t index);
	ApexButton* GetButton(size_t index);

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

private:
	void DrawBackground(sf::RenderTarget& target, sf::RenderStates states);
	void CreateBGTransition(size_t buttonIndex);

	std::map<size_t, ApexButton*> m_Buttons;
	TransformableTransition m_CurrentlyHighlightedButtonBackground;
	int m_CurrentlyHighlightedButtonIndex;

};
