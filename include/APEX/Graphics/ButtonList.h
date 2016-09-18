#pragma once

#include <APEX\Transition\TransformableTransition.h>
#include <APEX\IO\KeyListener.h>
#include <APEX\enumerations.h>

#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\System\Time.hpp>

#include <map>

namespace apex
{
	class Button;

	class ButtonList : public KeyListener
	{
	public:
		ButtonList();
		virtual ~ButtonList();

		ButtonList(const ButtonList&) = delete;
		ButtonList& operator=(const ButtonList&) = delete;

		void AddButton(Button* button, size_t index);
		Button* GetButton(size_t index);
		Button* GetHighlightedButton();
		void SetHighlightedButton(Button* highlightedButton);

		void Tick(sf::Time elapsed);
		void Draw(sf::RenderTarget& target, sf::RenderStates states);

		void Clear();

		void SetActive(bool active);

		virtual bool OnKeyPress(Keyboard::Key key, bool keyPressed) override;
		virtual bool OnUnmappedKeyPress(int vkCode, bool keyPressed) override;
		virtual void OnKeyRelease(Keyboard::Key key) override;

	private:
		bool CheckHighlightedButtonNeighbor(Direction direction);
		void DrawBackground(sf::RenderTarget& target, sf::RenderStates states);
		void CreateBGTransition(size_t buttonIndex);

		std::map<size_t, Button*> m_Buttons;
		TransformableTransition m_HighlightedButtonBackground;
		int m_HighlightedButtonIndex;
		bool m_KeyboardTouched;
		bool m_Active;

	};
} // namespace apex