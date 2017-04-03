
#include "Game\Debug\ImGui.h"
#include "Game\Game.h"

#include "Apex\ApexMain.h"
#include "Apex\IO\Mouse.h"

#include <SFML\Graphics\Text.hpp>

sf::Vector2f ImGui::s_TEMPORARY_windowTopLeft = sf::Vector2f(30, 30);

ImGuiStorage ImGui::s_Storage;
ImGuiID ImGui::s_DraggingID;

sf::Vector2f ImGui::s_CurrentPosition = ImGui::s_TEMPORARY_windowTopLeft;
sf::Vector2f ImGui::s_LastPosition;
sf::Font& ImGui::s_Font = Game::FontPixelFJ8;
unsigned int ImGui::s_CharacterSize;
unsigned int ImGui::s_LineHeight;

sf::Color ImGui::s_WindowBackgroundColor;
sf::Color ImGui::s_TextColor;
sf::Color ImGui::s_TextHoverColor;
sf::Color ImGui::s_ItemBackgroundColor;

sf::Vector2f ImGui::s_Padding;
sf::Vector2f ImGui::s_Bounds;
float ImGui::s_Opacity;
float ImGui::s_IndentAmount;
float ImGui::s_CornerRadius;

sf::Shader ImGui::s_RoundRectShader;

std::vector<ImGui::DrawableItem> ImGui::s_DrawList;

ImGui::ImGui()
{
}

ImGui::~ImGui()
{
}

void ImGui::Init()
{
	SetOpacity(0.75f);
	SetCharacterSize(30);
	SetTextColor(sf::Color(230, 232, 234));
	SetTextHoverColor(sf::Color(254, 253, 255));
	SetWindowBackgroundColor(sf::Color(25, 28, 31));
	SetPadding(sf::Vector2f(30, 30));
	SetIndentAmount(25.0f);
	SetCornerRadius(15.0f);

	if (!s_RoundRectShader.loadFromFile("resources/shaders/round_rectangle.frag", sf::Shader::Type::Fragment))
	{
		apex::PrintString("Could not load round rect shader!\n", apex::LogType::LOG_ERROR);
	}
	s_RoundRectShader.setUniform("u_resolution", static_cast<sf::Vector2f>(APEX->GetWindowSize()));
	s_RoundRectShader.setUniform("u_radius", s_CornerRadius);
}

void ImGui::SetFont(const sf::Font& font)
{
	s_Font = font;
}

void ImGui::SetCharacterSize(unsigned int characterSize)
{
	s_CharacterSize = characterSize;
	s_LineHeight = (unsigned int)(characterSize*1.3f);
}

void ImGui::SetPadding(sf::Vector2f padding)
{
	s_Padding = padding;
}

void ImGui::SetWindowBackgroundColor(sf::Color windowBackgroundColor)
{
	s_WindowBackgroundColor = windowBackgroundColor;
}

void ImGui::SetTextColor(sf::Color textColor)
{
	s_TextColor = textColor;
}

void ImGui::SetTextHoverColor(sf::Color textHoverColor)
{
	s_TextHoverColor = textHoverColor;
}

void ImGui::SetOpacity(float opacity)
{
	s_Opacity = std::max(0.0f, std::min(1.0f, opacity));
}

void ImGui::SetIndentAmount(float indentAmount)
{
	s_IndentAmount = indentAmount;
}

void ImGui::SetCornerRadius(float cornerRadius)
{
	s_CornerRadius = cornerRadius;
	s_RoundRectShader.setUniform("u_radius", s_CornerRadius);
}

void ImGui::SetLineHeight(unsigned int lineHeight)
{
	s_LineHeight = lineHeight;
}

bool ImGui::Text(const std::string& string)
{
	sf::Text* text = new sf::Text(string, s_Font, s_CharacterSize);
	sf::Color textColor = s_TextColor;
	textColor.a = sf::Uint8(s_Opacity * 255);
	text->setFillColor(textColor);
	text->setPosition(s_CurrentPosition);

	const float currentWidth = text->getLocalBounds().width;
	s_Bounds.x = std::max(s_Bounds.x, s_CurrentPosition.x + currentWidth);
	s_Bounds.y = std::max(s_Bounds.y, s_CurrentPosition.y);
	
	AddDrawable(text);

	return true;
}

bool ImGui::CollapsibleElement(const std::string& string)
{
	static const float halfTriangleSize = 10.0f;

	const ImGuiID id = ImGuiStorage::GetID(string.c_str());
	bool isCollapsed = s_Storage.GetBool(id);

	sf::ConvexShape* triangle = new sf::ConvexShape(3);
	triangle->setPoint(0, s_CurrentPosition);
	if (isCollapsed)
	{
		triangle->setPoint(1, s_CurrentPosition + sf::Vector2f(halfTriangleSize * 2, halfTriangleSize));
		triangle->setPoint(2, s_CurrentPosition + sf::Vector2f(0, halfTriangleSize * 2));
	}
	else
	{
		triangle->setPoint(1, s_CurrentPosition + sf::Vector2f(halfTriangleSize * 2, 0));
		triangle->setPoint(2, s_CurrentPosition + sf::Vector2f(halfTriangleSize, halfTriangleSize*2));
	}
	sf::FloatRect bounds(s_CurrentPosition, sf::Vector2f(halfTriangleSize*3.0f, halfTriangleSize*3.0f));
	bool isHovering = IsMouseHovering(bounds);

	if (isHovering) SetTextColor(s_TextHoverColor);
	sf::Color textColor = s_TextColor;
	if (!isHovering) textColor.a = sf::Uint8(s_Opacity * 255);
	triangle->setFillColor(textColor);
	AddDrawable(triangle);

	SameLine();
	Indent();

	sf::Text* text = new sf::Text(string, s_Font, s_CharacterSize);
	text->setFillColor(textColor);
	text->setPosition(s_CurrentPosition);

	const float currentWidth = text->getLocalBounds().width;
	s_Bounds.x = std::max(s_Bounds.x, s_CurrentPosition.x + currentWidth);
	s_Bounds.y = std::max(s_Bounds.y, s_CurrentPosition.y);

	AddDrawable(text);

	Indent();

	if (isHovering)
	{
		if (apex::Mouse::IsButtonPressed(sf::Mouse::Button::Left))
		{
			s_Storage.SetBool(id, !isCollapsed);
		}
	}
	
	Unindent();

	return !isCollapsed;
}

void ImGui::CollapsibleElementPop()
{
	Unindent();
	NewLine();
}

bool ImGui::RGBSlider(const std::string& label, sf::Color& color, float width)
{
	const float height = 40.0f;
	const float padding = 10.0f;
	const float bgWidth = (width == 0.0f ? height : width);
	const sf::Vector2f rectSize = sf::Vector2f(bgWidth, height);

	Text(label);

	sf::Color bgColor = s_ItemBackgroundColor;
	bgColor.a = sf::Uint8(s_Opacity * 255);
	sf::RectangleShape* bgRect = new sf::RectangleShape(sf::Vector2f((bgWidth + padding) * 4.0f + padding * 2, height + padding * 2));
	bgRect->setFillColor(bgColor);
	bgRect->setPosition(s_CurrentPosition);

	sf::Vector2f rRectPos = s_CurrentPosition + sf::Vector2f(padding, padding);
	sf::RectangleShape* rRect = new sf::RectangleShape(rectSize);
	rRect->setFillColor(sf::Color(color.r, 0, 0));
	rRect->setPosition(rRectPos);

	sf::Text* rText = new sf::Text(std::to_string(color.r), s_Font, 16);
	rText->setPosition(rRectPos);

	sf::Vector2f gRectPos = rRectPos + sf::Vector2f(bgWidth + padding, 0);
	sf::RectangleShape* gRect = new sf::RectangleShape(rectSize); 
	gRect->setFillColor(sf::Color(0, color.g, 0));
	gRect->setPosition(gRectPos);

	sf::Text* gText = new sf::Text(std::to_string(color.g), s_Font, 16);
	gText->setPosition(gRectPos);

	sf::Vector2f bRectPos = gRectPos + sf::Vector2f(bgWidth + padding, 0);
	sf::RectangleShape* bRect = new sf::RectangleShape(rectSize);
	bRect->setFillColor(sf::Color(0, 0, color.b));
	bRect->setPosition(bRectPos);

	sf::Text* bText = new sf::Text(std::to_string(color.b), s_Font, 16);
	bText->setPosition(bRectPos);

	sf::Vector2f rgbRectPos = bRectPos + sf::Vector2f(bgWidth + padding, -padding);
	sf::RectangleShape* rgbRect = new sf::RectangleShape(sf::Vector2f(height + padding * 2, height + padding * 2));
	rgbRect->setFillColor(color);
	rgbRect->setPosition(rgbRectPos);

	const sf::Vector2i drag = apex::Mouse::GetDraggedVector();
	const bool dragging = drag != sf::Vector2i(0, 0);
	const ImGuiID previousDraggingID = s_DraggingID;
	const bool lmbPressed = apex::Mouse::IsButtonPressed(sf::Mouse::Button::Left);
	const bool lmbReleased = apex::Mouse::IsButtonReleased(sf::Mouse::Button::Left);

	const ImGuiID rID = ImGuiStorage::GetID(std::string(label + ",c:r").c_str());
	const ImGuiID gID = ImGuiStorage::GetID(std::string(label + ",c:g").c_str());
	const ImGuiID bID = ImGuiStorage::GetID(std::string(label + ",c:b").c_str());

	const sf::Uint8 hoverColor = 60;

	if (CheckColorSwatchHover(rID, rRect->getLocalBounds(), rRectPos, rectSize, padding))
	{
		rRect->setFillColor(sf::Color(color.r, hoverColor, hoverColor));
	}

	if (CheckColorSwatchHover(gID, gRect->getLocalBounds(), gRectPos, rectSize, padding))
	{
		gRect->setFillColor(sf::Color(hoverColor, color.g, hoverColor));
	}

	if (CheckColorSwatchHover(bID, bRect->getLocalBounds(), bRectPos, rectSize, padding))
	{
		bRect->setFillColor(sf::Color(hoverColor, hoverColor, color.b));
	}

	if (lmbReleased)
	{
		s_Storage.ClearValue(rID);
		s_Storage.ClearValue(gID);
		s_Storage.ClearValue(bID);
	}

	const int deltaColor = int((float(drag.x) / APEX->GetWindowSize().x) * 500);

	if (dragging)
	{
		color.r = GetColorSwatchValue(rID, color.r, deltaColor);
		color.g = GetColorSwatchValue(gID, color.g, deltaColor);
		color.b = GetColorSwatchValue(bID, color.b, deltaColor);
	}

	AddDrawable(bgRect); SameLine();
	AddDrawable(rRect); SameLine();
	AddDrawable(rText); SameLine();
	AddDrawable(gRect); SameLine();
	AddDrawable(gText); SameLine();
	AddDrawable(bRect); SameLine();
	AddDrawable(bText); SameLine();
	AddDrawable(rgbRect);
	NewLine();

	return true;
}

bool ImGui::FloatSlider(const std::string& label, float* value, float min, float max, float width)
{
	const float padding = 10.0f;
	const float bgWidth = (width == 0.0f? 100.0f : width);
	const float bgHeight = 40.0f;
	const sf::Vector2f bgSize(bgWidth, bgHeight);

	sf::RectangleShape* bgRect = new sf::RectangleShape(bgSize);
	sf::Color bgColor = s_ItemBackgroundColor;
	bgColor.a = sf::Uint8(s_Opacity * 255);
	bgRect->setFillColor(bgColor);
	bgRect->setPosition(s_CurrentPosition);

	sf::RectangleShape* handleRect = new sf::RectangleShape(sf::Vector2f(bgWidth * 0.1f, bgHeight - padding * 2));
	sf::Color handleColor = sf::Color(180, 185, 190);
	handleColor.a = sf::Uint8(s_Opacity * 255);
	handleRect->setFillColor(handleColor);
	handleRect->setPosition(s_CurrentPosition + sf::Vector2f(padding + (*value) * (bgWidth-padding), padding));

	AddDrawable(bgRect);
	AddDrawable(handleRect);

	return false;
}

bool ImGui::CheckColorSwatchHover(ImGuiID id, sf::FloatRect bounds, sf::Vector2f rectPos, sf::Vector2f rectSize, float padding)
{
	bounds.left += rectPos.x;
	bounds.top += rectPos.y;
	if (IsMouseHovering(bounds))
	{
		if (apex::Mouse::IsButtonPressed(sf::Mouse::Left) && s_DraggingID == 0)
		{
			s_DraggingID = id;
		}
		else
		{
			if (s_DraggingID == 0)
			{
				return true;
			}
		}
	}
	return false;
}

sf::Uint8 ImGui::GetColorSwatchValue(ImGuiID swatchID, sf::Uint8 swatchValue, int deltaColor)
{
	if (s_DraggingID == swatchID)
	{
		const int valueOnClick = s_Storage.GetInt(swatchID);
		if (valueOnClick == -1)
		{
			// Set the value for the first time
			s_Storage.SetInt(swatchID, swatchValue);
			return swatchValue;
		}
		else
		{
			// It was already set, the new value will be relative to it
			return sf::Uint8(std::max(0, std::min(255, valueOnClick + deltaColor)));
		}
	}
	return swatchValue;
}

void ImGui::Indent(float indentAmount)
{
	s_CurrentPosition.x += (indentAmount == 0.0f ? s_IndentAmount : indentAmount);
}

void ImGui::Unindent(float indentAmount)
{
	s_CurrentPosition.x -= (indentAmount == 0.0f ? s_IndentAmount : indentAmount);
}

void ImGui::SameLine(float x)
{
	s_CurrentPosition = s_LastPosition;
	if (x != 0.0f)
	{
		s_CurrentPosition.x = x;
	}
}

void ImGui::NewLine()
{
	s_CurrentPosition.y += s_LineHeight;
	s_Bounds.y = std::max(s_Bounds.y, s_CurrentPosition.y - s_Padding.y);
}

void ImGui::AddDrawable(sf::Drawable* drawable)
{
	DrawableItem item;
	item.drawable = drawable;
	AddDrawable(item);
}

void ImGui::AddDrawable(DrawableItem drawable)
{
	s_DrawList.push_back(drawable);

	s_LastPosition = s_CurrentPosition;
	s_CurrentPosition.y += s_LineHeight;
}

bool ImGui::IsMouseHovering(sf::FloatRect bounds)
{
	const sf::Vector2i mousePos = APEX->GetMouseCoordsScreenSpace();
	return mousePos.x >= bounds.left && mousePos.x < bounds.left + bounds.width &&
		mousePos.y >= bounds.top && mousePos.y < bounds.top + bounds.height;
}

void ImGui::Draw(sf::RenderTarget& target)
{
	const sf::View previousView = target.getView();

	target.setView(target.getDefaultView());
	sf::RenderStates states = sf::RenderStates::Default;

	sf::RectangleShape bounds;
	bounds.setPosition(s_TEMPORARY_windowTopLeft);
	bounds.setSize(s_Bounds + s_Padding * 2.0f);
	sf::Color backgroundColor = s_WindowBackgroundColor;
	backgroundColor.a = sf::Uint8(s_Opacity * 255);
	bounds.setFillColor(backgroundColor);
	states.shader = &s_RoundRectShader;
	s_RoundRectShader.setUniform("u_pos", bounds.getPosition());
	s_RoundRectShader.setUniform("u_size", bounds.getSize());
	target.draw(bounds, states);
	states.shader = sf::RenderStates::Default.shader;

	states.transform.translate(s_Padding);
	std::vector<DrawableItem>::iterator it;
	for (it = s_DrawList.begin(); it != s_DrawList.end(); ++it)
	{
		target.draw(*(it->drawable));//, it->states);
		delete it->drawable;
	}
	s_DrawList.clear();

	s_CurrentPosition = s_TEMPORARY_windowTopLeft + s_Padding;
	s_LastPosition = s_CurrentPosition;
	s_Bounds.x = 0.0f;//s_Padding.x * 2;
	s_Bounds.y = 0.0f;//s_Padding.y * 2;

	target.setView(previousView);
}

std::string ImGui::ToString(bool value)
{
	return (value ? "true" : "false");
}

std::string ImGui::ToString(float value, int precision)
{
	std::stringstream ss;
	if (abs(value < 0.00001f)) value = 0.0f;
	ss << std::setprecision(precision) << value;
	return ss.str();
}

std::string ImGui::ToString(sf::Vector2f value, const std::string& separator, int precision)
{
	std::stringstream ss;
	if (abs(value.x) < 0.00001f) value.x = 0.0f;
	if (abs(value.y) < 0.00001f) value.y = 0.0f;
	ss << std::setprecision(precision) << value.x << separator << value.y;
	return ss.str();
}

bool ImGui::OnButtonPress(sf::Event::MouseButtonEvent buttonEvent)
{
	if (buttonEvent.button == sf::Mouse::Button::Right)
	{
		if (s_DraggingID != 0) // The user wants to cancel their current action
		{
			s_DraggingID = 0;
		}
	}
	return false;
}

void ImGui::OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent)
{
	s_DraggingID = 0;
}

bool ImGui::OnKeyPress(apex::Keyboard::Key key, bool keyPressed)
{
	return false;
}

bool ImGui::OnKeyRelease(apex::Keyboard::Key)
{
	return false;
}

