#pragma once

#include "ApexKeyListener.h"
#include "ApexButtonList.h"

#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\Text.hpp>
#include <SFML\System\Time.hpp>

class World;
class ApexButton;
class ApexSlider;

class PauseScreen : public ApexKeyListener
{
public:
	enum class Screen
	{
		MAIN, OPTIONS, KEYBINDINGS, NONE
	};

	PauseScreen(World& world);
	virtual ~PauseScreen();

	PauseScreen(const PauseScreen&) = delete;
	PauseScreen& operator=(const PauseScreen&) = delete;

	void Draw(sf::RenderTarget& target, sf::RenderStates states);
	void Tick(sf::Time elapsed);

	void SetScreenShowing(Screen screen);

	void OnUnmappedKeyPress(sf::Event::KeyEvent event);

	virtual bool OnKeyPress(ApexKeyboard::Key key, bool keyPressed) override;
	virtual void OnKeyRelease(ApexKeyboard::Key key) override;

private:
	struct Keybinding
	{
		sf::Text label;
		ApexKeyboard::Key key;
		ApexButton* button = nullptr;
	};

	enum class MainScreenButtons
	{
		RESUME, OPTIONS, MAIN_MENU,
		_LAST_ELEMENT
	};
	enum class OptionsScreenButtons
	{
		FULLSCREEN, ENABLE_V_SYNC, RENDER_LIGHTING, KEYBINDINGS,
		_LAST_ELEMENT
	};

	struct ApexOptions
	{
		bool fullscreen = false;
		bool useVSync = false;
		bool renderLighting = true;
		float musicVolume = 0.5f;
		float soundVolume = 0.5f;
		std::vector<Keybinding> keybindings;

		// Limit FPS
		// Graphics quality
		// GUI Scale
		// Transition speed
		// Show physics debug
		// Use custom cursor
		// Language (???)
		// Flip mouse-y
		// Brightness
		// Resolution
		// Text scrolling speed
	};

	void AssignKeybinding(Keybinding& keybinding, int vkCode);
	void SaveOptionsToFile();
	void ReadOptionsFromFile();
	template<typename T>
	T FindOption(const nlohmann::json& options, const std::string& option);

	// Main screen:
	ApexButtonList m_MainScreenButtonList;

	// Options screen:
	ApexOptions m_Options;
	ApexButtonList m_OptionsButtonList;
	ApexSlider* m_MusicVolumeSlider = nullptr;
	sf::Text m_MusicVolumeText;
	ApexSlider* m_SoundVolumeSlider = nullptr;
	sf::Text m_SoundVolumeText;
	bool m_SoundVolumeSliderWasBeingDragged;

	// Keybindings screen:
	int m_KeybindingAssigningIndex = -1;

	World& m_World;

	Screen m_CurrentScreenShowing;

};
