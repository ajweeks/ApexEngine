#pragma once

#include "ButtonList.h"

#include "Apex\IO\KeyListener.h"
#include "Apex\State\StateManager.h"

#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\Text.hpp>
#include <SFML\System\Time.hpp>

class World;

namespace apex
{
	class Button;
	class Slider;
} // namespace apex

class PauseScreen : public apex::KeyListener
{
public:
	enum class Screen
	{
		MAIN, OPTIONS, KEYBINDINGS, NONE
	};

	PauseScreen(World& world, apex::StateManager& stateManager);
	virtual ~PauseScreen();

	PauseScreen(const PauseScreen&) = delete;
	PauseScreen& operator=(const PauseScreen&) = delete;

	void Draw(sf::RenderTarget& target, sf::RenderStates states);
	void Tick(sf::Time elapsed);

	void SetScreenShowing(Screen screen);

	void OnUnmappedKeyPress(sf::Event::KeyEvent event);

	virtual bool OnKeyPress(apex::Keyboard::Key key, bool keyPressed) override;
	virtual void OnKeyRelease(apex::Keyboard::Key key) override;

private:
	struct Keybinding
	{
		sf::Text label;
		apex::Keyboard::Key key;
		apex::Button* button = nullptr;
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
	ButtonList m_MainScreenButtonList;

	// Options screen:
	ApexOptions m_Options;
	ButtonList m_OptionsButtonList;
	apex::Slider* m_MusicVolumeSlider = nullptr;
	sf::Text m_MusicVolumeText;
	apex::Slider* m_SoundVolumeSlider = nullptr;
	sf::Text m_SoundVolumeText;
	bool m_SoundVolumeSliderWasBeingDragged;

	// Keybindings screen:
	int m_KeybindingAssigningIndex = -1;

	World& m_World;
	apex::StateManager& m_StateManager;

	Screen m_CurrentScreenShowing;

};