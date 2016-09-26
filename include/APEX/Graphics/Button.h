#pragma once

#include <APEX\IO\MouseListener.h>
#include <APEX\enumerations.h>

#include <SFML\Graphics\RectangleShape.hpp>
#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\Text.hpp>

#include <array>

namespace apex
{
	class Button : public MouseListener
	{
	public:
		static const sf::Color FONT_COLOR;
		static const sf::Color BG_COLOR;
		static const sf::Color BG_HOVER_COLOR;

		Button(float left, float top, float width, float height, std::string text, const sf::Font& font, unsigned int characterSize = 48);
		virtual ~Button();

		Button(const Button&) = delete;
		Button& operator=(const Button&) = delete;

		void Tick(sf::Time elapsed);
		void Draw(sf::RenderTarget& target, sf::RenderStates states);

		void SetFillColour(sf::Color fillColour);
		void SetTextColour(sf::Color textColour);

		bool IsDown() const;
		bool IsPressed() const;
		void SetString(size_t index, sf::String string);
		void AddString(sf::String string);
		void ShowNextString();

		sf::String GetCurrentString() const;
		sf::Vector2f GetPosition() const;

		void SetShowingBackground(bool show);

		void SetActive(bool active);
		void ClearInputs();
		sf::RectangleShape GetRectangle() const;

		bool IsHovering() const;

		void SetNeighbor(Button* neighbor, Direction direction);
		void SetNeighbors(Button* up, Button* down, Button* left, Button* right);
		Button* GetNeighbor(Direction direction);

		virtual bool OnButtonPress(sf::Event::MouseButtonEvent buttonEvent);
		virtual void OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent);
		virtual void OnScroll(sf::Event::MouseWheelScrollEvent scrollEvent);

	private:
		void SetHovering(bool hovering);

		sf::RectangleShape m_BoundingRect;
		sf::Color m_FillColour;
		sf::Color m_HoverFillColour;
		sf::Color m_TextColour;
		sf::Text m_Text;
		std::vector<sf::String> m_StringOptions; // Typically just one string, but can hold multiple and cycle through on click
		int m_CurrentStringIndex;

		std::array<Button*, 4> m_Neighbors; // Used for keyboard/controller navigation

		bool m_Hovering = false;
		bool m_IsDown = false;
		bool m_IsPressed = false;

		bool m_ShowBackground = true;
	};
} // namespace apex
