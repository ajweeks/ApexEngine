
#include "ApexPauseScreen.h"
#include "Level.h"
#include "GameState.h"
#include "ApexMain.h"
#include "MainMenuState.h"
#include "ApexMouse.h"
#include "ApexButton.h"
#include "ApexSlider.h"
#include "ApexAudio.h"

#include <SFML\Graphics\RectangleShape.hpp>

ApexPauseScreen::ApexPauseScreen(Level* level) :
	m_Level(level)
{
	const float width = 300.0f;
	const float height = 120.0f;
	const float centerX = APEX->GetWindowSize().x / 2.0f;
	float left = centerX - width / 2.0f;
	float top = 550.0f;

	// Main screen
	m_ResumeButton = new ApexButton(left, top, width, height, "Resume");
	top += height * 1.5f;
	m_OptionsButton = new ApexButton(left, top, width, height, "Options");
	top += height * 1.5f;
	m_MainMenuButton = new ApexButton(left, top, width, height, "Main Menu");

	// Options screen
	top = 550.0f;
	m_FullscreenButton = new ApexButton(left, top, width, height, "Fullscreen");
	m_FullscreenButton->AddString("Windowed");

	top += height * 1.5f;
	m_MusicVolumeText = sf::Text("Music Volume:", APEX->FontOpenSans, 38);
	m_MusicVolumeText.setColor(sf::Color(210, 210, 210));
	m_MusicVolumeText.setPosition(left, top);

	const float volumeBarWidth = width * 1.2f;
	const float volumeBarHeight = height * 0.25f;
	top += height * 0.5f;
	m_MusicVolumeSlider = new ApexSlider(volumeBarWidth, volumeBarHeight, centerX - volumeBarWidth / 2, top);

	top += height * 0.8f;
	m_SoundVolumeText = sf::Text("Sound Volume:", APEX->FontOpenSans, 38);
	m_SoundVolumeText.setColor(sf::Color(210, 210, 210));
	m_SoundVolumeText.setPosition(left, top);

	top += height * 0.5f;
	m_SoundVolumeSlider = new ApexSlider(volumeBarWidth, volumeBarHeight, centerX - volumeBarWidth / 2, top);

	m_CurrentScreenShowing = Screen::MAIN;
}

ApexPauseScreen::~ApexPauseScreen()
{
	delete m_ResumeButton;
	delete m_OptionsButton;
	delete m_MainMenuButton;
	delete m_FullscreenButton;
	delete m_MusicVolumeSlider;
	delete m_SoundVolumeSlider;
}

void ApexPauseScreen::Tick(sf::Time elapsed)
{
	switch (m_CurrentScreenShowing)
	{
	case Screen::MAIN:
	{
		m_MainMenuButton->Tick(elapsed);
		if (m_ResumeButton->IsDown())
		{
			m_Level->TogglePaused(true);
		}

		m_OptionsButton->Tick(elapsed);
		if (m_OptionsButton->IsDown())
		{
			SetScreenShowing(Screen::OPTIONS);
		}

		m_ResumeButton->Tick(elapsed);
		if (m_MainMenuButton->IsDown())
		{
			APEX->GetStateManager()->SetState(new MainMenuState());
		}
	} break;
	case Screen::OPTIONS:
	{
		m_FullscreenButton->Tick(elapsed);
		if (m_FullscreenButton->IsPressed())
		{
			m_MainMenuButton->ClearInputs();
			ApexMouse::Clear();
			SetScreenShowing(Screen::MAIN);
			APEX->ToggleWindowFullscreen();
			return;
		}

		m_MusicVolumeSlider->Tick(elapsed);
		ApexAudio::SetAllMusicVolume(m_MusicVolumeSlider->GetValue());

		m_SoundVolumeSlider->Tick(elapsed);
		ApexAudio::SetAllSoundsVolume(m_SoundVolumeSlider->GetValue());
	} break;
	}
}

void ApexPauseScreen::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	sf::RectangleShape bgRect(static_cast<sf::Vector2f>(target.getSize()));
	bgRect.setFillColor(sf::Color(25, 25, 40, 140));
	target.draw(bgRect);

	switch (m_CurrentScreenShowing)
	{
	case Screen::MAIN:
	{
		m_ResumeButton->Draw(target, states);
		m_OptionsButton->Draw(target, states);
		m_MainMenuButton->Draw(target, states);
	} break;
	case Screen::OPTIONS:
	{
		m_FullscreenButton->Draw(target, states);

		target.draw(m_MusicVolumeText, states);
		m_MusicVolumeSlider->Draw(target, states);

		target.draw(m_SoundVolumeText, states);
		m_SoundVolumeSlider->Draw(target, states);
	} break;
	}
}

void ApexPauseScreen::SetScreenShowing(Screen screen)
{
	m_CurrentScreenShowing = screen;

	m_MainMenuButton->ClearInputs();
	m_OptionsButton->ClearInputs();
	m_ResumeButton->ClearInputs();
	m_FullscreenButton->ClearInputs();
}
