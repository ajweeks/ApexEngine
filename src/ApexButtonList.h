#pragma once

#include "TransformableTransition.h"
#include "ApexKeyListener.h"
#include "enumerations.h"

#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\System\Time.hpp>

#include <map>

class ApexButton;

class ApexButtonList : public ApexKeyListener
{
public:
	ApexButtonList();
	virtual ~ApexButtonList();

	ApexButtonList(const ApexButtonList&) = delete;
	ApexButtonList& operator=(const ApexButtonList&) = delete;

	void AddButton(ApexButton* button, size_t index);
	ApexButton* GetButton(size_t index);
	ApexButton* GetHighlightedButton();
	void SetHighlightedButton(ApexButton* highlightedButton);

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

	void Clear();

	void SetActive(bool active);

	virtual bool OnKeyPress(ApexKeyboard::Key key, bool keyPressed) override;
	virtual bool OnUnmappedKeyPress(int vkCode, bool keyPressed) override;
	virtual void OnKeyRelease(ApexKeyboard::Key key) override;

private:
	bool CheckHighlightedButtonNeighbor(Direction direction);
	void DrawBackground(sf::RenderTarget& target, sf::RenderStates states);
	void CreateBGTransition(size_t buttonIndex);

	std::map<size_t, ApexButton*> m_Buttons;
	TransformableTransition m_HighlightedButtonBackground;
	int m_HighlightedButtonIndex;
	bool m_KeyboardTouched;
	bool m_Active;

};
