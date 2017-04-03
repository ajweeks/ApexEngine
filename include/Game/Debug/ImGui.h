#pragma once

#include "Game\Debug\ImGuiStorage.h"

#include "Apex\IO\Keyboard.h"

#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\Drawable.hpp>
#include <SFML\Graphics\Font.hpp>
#include <SFML\Graphics\Shader.hpp>
#include <SFML\Window\Event.hpp>

#include <string>
#include <vector>

class ImGui
{
public:
	static void Init();

	static void SetFont(const sf::Font& font);
	static void SetCharacterSize(unsigned int characterSize);
	static void SetLineHeight(unsigned int lineHeight);
	static void SetTextColor(sf::Color textColor);
	static void SetTextHoverColor(sf::Color textHoverColor);
	static void SetWindowBackgroundColor(sf::Color windowBackgroundColor);
	//static void SetTextBoxBackgroundColor(sf::Color textBoxBackgroundColor);
	//static void SetTextBoxTextColor(sf::Color textBoxTextColor);
	static void SetPadding(sf::Vector2f padding);
	static void SetOpacity(float opacity);
	static void SetIndentAmount(float indentAmount);
	static void SetCornerRadius(float cornerRadius);

	static bool Text(const std::string& string);
	static bool CollapsibleElement(const std::string& string); // Returns true if this element is not collapsed
	static void CollapsibleElementPop();
	static bool RGBSlider(const std::string& label, sf::Color& color, float width = 0.0f);
	static bool FloatSlider(const std::string& label, float* value, float min, float max, float width = 0.0f);
	//static bool TextBox(const std::string& label, const std::string& string, float width = 0.0f); // Creates a single line text box

	static void Indent(float indentAmount = 0.0f); // Move content to the right by s_IndentAmount if indentAmount == 0, or the value specified
	static void Unindent(float indentAmount = 0.0f); // Move content to the left by s_IndentAmount if indentAmount == 0, or the value specified
	static void SameLine(float x = 0.0f); // Moves back to previous line and continues on where left off if x == 0, else translate across by x
	static void NewLine(); // Leaves an empty line

	static void Draw(sf::RenderTarget& target);

	static std::string ToString(bool value);
	static std::string ToString(float value, int precision = 4);
	static std::string ToString(sf::Vector2f, const std::string& separator = ", ", int precision = 4);

	static bool OnButtonPress(sf::Event::MouseButtonEvent buttonEvent);
	static void OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent);

	static bool OnKeyPress(apex::Keyboard::Key key, bool keyPressed);
	static bool OnKeyRelease(apex::Keyboard::Key);

private:
	ImGui();
	virtual ~ImGui();

	ImGui(const ImGui&) = delete;
	ImGui& operator=(const ImGui&) = delete;

	struct DrawableItem
	{
		sf::Drawable* drawable = nullptr;
		sf::RenderStates states = sf::RenderStates::Default;
	};

	static void AddDrawable(sf::Drawable* drawable); // Adds a drawable item with default render states
	static void AddDrawable(DrawableItem drawable);
	static bool IsMouseHovering(sf::FloatRect bounds);
	// Returns true if this swatch should use its hover color (if its being hovered over and nothing is being dragged)
	static bool CheckColorSwatchHover(ImGuiID id, sf::FloatRect bounds, sf::Vector2f rectPos, sf::Vector2f rectSize, float padding);
	// Checks for a value in storage, returns it plus deltaColor if found, sets it to swatchValue if not
	static sf::Uint8 GetColorSwatchValue(ImGuiID swatchID, sf::Uint8 swatchValue, int deltaColor);

	static std::vector<DrawableItem> s_DrawList;

	static sf::Vector2f s_TEMPORARY_windowTopLeft;

	static ImGuiStorage s_Storage;
	static ImGuiID s_CurrentWindowID; // Set on Begin() and unset on End()
	static ImGuiID s_DraggingID; // When the mouse is held down

	static sf::Vector2f s_LastPosition;
	static sf::Vector2f s_CurrentPosition;
	static sf::Font& s_Font;
	static unsigned int s_CharacterSize;
	static unsigned int s_LineHeight;
	
	static sf::Color s_WindowBackgroundColor;
	static sf::Color s_TextColor;
	static sf::Color s_TextHoverColor;
	static sf::Color s_ItemBackgroundColor;
	//static sf::Color s_TextBoxBackgroundColor;
	//static sf::Color s_TextBoxTextColor;

	static sf::Vector2f s_Bounds;
	static sf::Vector2f s_Padding;
	static float s_Opacity;
	static float s_IndentAmount;
	static float s_CornerRadius;

	static sf::Shader s_RoundRectShader;

};
