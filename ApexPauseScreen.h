#pragma once

#include "ApexKeyListener.h"

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
	void AssignKeybinding(Keybinding& keybinding, int vkCode);

	// Main screen:
	ApexButton* m_ResumeButton = nullptr;
	ApexButton* m_OptionsButton = nullptr;
	ApexButton* m_MainMenuButton = nullptr;

	// Options screen:
	ApexButton* m_FullscreenButton = nullptr;
	sf::Text m_MusicVolumeText;
	ApexSlider* m_MusicVolumeSlider = nullptr;
	sf::Text m_SoundVolumeText;
	ApexSlider* m_SoundVolumeSlider = nullptr;
	bool m_SoundVolumeSliderWasBeingDragged;
	ApexButton* m_KeybindingsButton = nullptr;
	// Brightness
	// Resolution
	// ...

	// Keybindings screen:

	std::vector<Keybinding> m_Keybindings;
	int m_KeybindingAssigningIndex = -1;

	World* m_World = nullptr;

	Screen m_CurrentScreenShowing;

};
