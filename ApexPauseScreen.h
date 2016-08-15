#pragma once

#include "ApexKeyListener.h"
#include "ApexButtonList.h"

#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\Text.hpp>
#include <SFML\System\Time.hpp>

class World;
class ApexButton;
class ApexSlider;

class ApexPauseScreen : public ApexKeyListener
{
public:
	enum class Screen
	{
		MAIN, OPTIONS, KEYBINDINGS
	};

	ApexPauseScreen(World* world);
	virtual ~ApexPauseScreen();

	ApexPauseScreen(const ApexPauseScreen&) = delete;
	ApexPauseScreen& operator=(const ApexPauseScreen&) = delete;

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
		RESUME, OPTIONS, MAIN_MENU
	};
	enum class OptionsScreenButtons
	{
		FULLSCREEN, KEYBINDINGS
	};

	void AssignKeybinding(Keybinding& keybinding, int vkCode);

	// Main screen:
	ApexButtonList m_MainScreenButtonList;

	// Options screen:
	ApexButtonList m_OptionsButtonList;
	ApexSlider* m_MusicVolumeSlider = nullptr;
	sf::Text m_MusicVolumeText;
	ApexSlider* m_SoundVolumeSlider = nullptr;
	sf::Text m_SoundVolumeText;
	bool m_SoundVolumeSliderWasBeingDragged;
	// Brightness
	// Resolution
	// ...

	// Keybindings screen:
	std::vector<Keybinding> m_Keybindings;
	int m_KeybindingAssigningIndex = -1;

	World* m_World = nullptr;

	Screen m_CurrentScreenShowing;

};
