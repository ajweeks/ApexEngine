#pragma once

#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\Text.hpp>
#include <SFML\System\Time.hpp>

class Level;
class ApexButton;
class ApexSlider;

class ApexPauseScreen
{
public:
	enum class Screen
	{
		MAIN, OPTIONS
	};

	ApexPauseScreen(Level* level);
	virtual ~ApexPauseScreen();

	ApexPauseScreen(const ApexPauseScreen&) = delete;
	ApexPauseScreen& operator=(const ApexPauseScreen&) = delete;

	void Draw(sf::RenderTarget& target, sf::RenderStates states);
	void Tick(sf::Time elapsed);

	void SetScreenShowing(Screen screen);

private:
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

	// Brightness?
	// Resolution
	// ...

	Level* m_Level = nullptr;

	Screen m_CurrentScreenShowing;

};
