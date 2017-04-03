
#pragma once

#include "Apex\Transition\TransformableTransition.h"
#include "Apex\IO\KeyListener.h"
#include "Apex\enumerations.h"

#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\System\Time.hpp>

#include <map>

namespace apex
{
	class Button;
}

class ButtonList : public apex::KeyListener
{
public:
	ButtonList();
	virtual ~ButtonList();

	ButtonList(const ButtonList&) = delete;
	ButtonList& operator=(const ButtonList&) = delete;

	void AddButton(apex::Button* button, size_t index);
	apex::Button* GetButton(size_t index);
	apex::Button* GetHighlightedButton();
	void SetHighlightedButton(apex::Button* highlightedButton);

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

	void Clear();

	void SetActive(bool active);

	virtual bool OnKeyPress(apex::Keyboard::Key key, bool keyPressed) override;
	virtual bool OnUnmappedKeyPress(int vkCode, bool keyPressed) override;
	virtual void OnKeyRelease(apex::Keyboard::Key key) override;

private:
	bool CheckHighlightedButtonNeighbor(apex::Direction direction);
	void DrawBackground(sf::RenderTarget& target, sf::RenderStates states);
	void CreateBGTransition(size_t buttonIndex);

	std::map<size_t, apex::Button*> m_Buttons;
	apex::TransformableTransition m_HighlightedButtonBackground;
	int m_HighlightedButtonIndex;
	bool m_KeyboardTouched;
	bool m_Active;

};
