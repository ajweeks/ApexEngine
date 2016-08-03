
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
	ApexKeyListener(),
	m_Level(level)
{
	const float width = 300.0f;
	const float height = 120.0f;
	const sf::Vector2u windowSize = APEX->GetWindowSize();
	const float centerX = windowSize.x / 2.0f;
	float left = centerX - width / 2.0f;
	const float initialTop = 550.0f;
	float top = initialTop;
	float dy = height * 1.5f;
	const float maxY = windowSize.y - height * 1.5f;

	const unsigned int fontSize = 38;
	const sf::Font& font = APEX->FontPixelFJ8;

	// Main screen
	m_ResumeButton = new ApexButton(left, top, width, height, "Resume");
	top += dy;
	m_OptionsButton = new ApexButton(left, top, width, height, "Options");
	top += dy;
	m_MainMenuButton = new ApexButton(left, top, width, height, "Main Menu");

	// Options screen
	top = initialTop;
	m_FullscreenButton = new ApexButton(left, top, width, height, "Fullscreen");
	m_FullscreenButton->AddString("Windowed");
	top += dy;
	m_KeybindingsButton = new ApexButton(left + (width / 2.0f - width * 0.8f), top, width * 1.6f, height, "Keyboard bindings");

	top += dy;
	m_MusicVolumeText = sf::Text("Music Volume:", font, fontSize);
	m_MusicVolumeText.setColor(sf::Color(210, 210, 210));
	m_MusicVolumeText.setPosition(left, top);

	const float volumeBarWidth = width * 1.2f;
	const float volumeBarHeight = height * 0.25f;
	top += height * 0.5f;
	m_MusicVolumeSlider = new ApexSlider(volumeBarWidth, volumeBarHeight, centerX - volumeBarWidth / 2, top);

	top += height * 0.8f;
	m_SoundVolumeText = sf::Text("Sound Volume:", font, fontSize);
	m_SoundVolumeText.setColor(sf::Color(210, 210, 210));
	m_SoundVolumeText.setPosition(left, top);

	top += height * 0.5f;
	m_SoundVolumeSlider = new ApexSlider(volumeBarWidth, volumeBarHeight, centerX - volumeBarWidth / 2, top);

	// Keybindings screen
	int vkCode;
	const int buttonXO = 300;
	const float smallButtonWidth = 160;
	const float smallButtonHeight = 70;
	dy = smallButtonHeight * 1.5f;
	top = 120;
	left = 220;
	for (size_t i = 0; i < ApexKeyboard::GetNumberOfKeys(); i++)
	{
		Keybinding keybinding;

		keybinding.label = sf::Text(ApexKeyboard::GetKeyName(ApexKeyboard::Key(i)), font, fontSize);
		keybinding.label.setPosition(left, top);
		keybinding.key = ApexKeyboard::Key(i);
		ApexKeyboard::GetUnMappedKey(keybinding.key, vkCode);
		keybinding.button = new ApexButton(left + buttonXO, top, smallButtonWidth, smallButtonHeight, 
			ApexKeyboard::GetSFKeyName(sf::Keyboard::Key(vkCode)));
		
		m_Keybindings.push_back(keybinding);

		top += dy;

		if (top > maxY) 
		{
			top = 120;
			left += width * 3.0f;
		}
	}

	m_KeybindingAssigningIndex = -1;

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
	delete m_KeybindingsButton;

	for (size_t i = 0; i < m_Keybindings.size(); i++)
	{
		delete m_Keybindings[i].button;
	}
	m_Keybindings.clear();
}

void ApexPauseScreen::Tick(sf::Time elapsed)
{
	switch (m_CurrentScreenShowing)
	{
	case Screen::MAIN:
	{
		m_ResumeButton->Tick(elapsed);
		if (m_ResumeButton->IsPressed())
		{
			m_Level->TogglePaused(true);
			return;
		}

		m_MainMenuButton->Tick(elapsed);
		if (m_MainMenuButton->IsPressed())
		{
			APEX->GetStateManager()->SetState(new MainMenuState());
			return;
		}

		m_OptionsButton->Tick(elapsed);
		if (m_OptionsButton->IsPressed())
		{
			SetScreenShowing(Screen::OPTIONS);
			return;
		}
	} break;
	case Screen::OPTIONS:
	{
		m_FullscreenButton->Tick(elapsed);
		if (m_FullscreenButton->IsPressed())
		{
			m_FullscreenButton->ClearInputs();
			ApexMouse::Clear();
			SetScreenShowing(Screen::MAIN);
			APEX->ToggleWindowFullscreen();
			return;
		}

		m_MusicVolumeSlider->Tick(elapsed);
		ApexAudio::SetAllMusicVolume(m_MusicVolumeSlider->GetValue());

		m_SoundVolumeSlider->Tick(elapsed);
		ApexAudio::SetAllSoundsVolume(m_SoundVolumeSlider->GetValue());
		const bool soundVolumeSliderBeingBragged = m_SoundVolumeSlider->BeingDragged();
		if (m_SoundVolumeSliderWasBeingDragged && !soundVolumeSliderBeingBragged)
		{
			ApexAudio::PlaySoundEffect(ApexAudio::Sound::BOOP);
		}
		m_SoundVolumeSliderWasBeingDragged = soundVolumeSliderBeingBragged;

		m_KeybindingsButton->Tick(elapsed);
		if (m_KeybindingsButton->IsPressed())
		{
			SetScreenShowing(Screen::KEYBINDINGS);
			return;
		}
	} break;
	case Screen::KEYBINDINGS:
	{
		for (size_t i = 0; i < m_Keybindings.size(); i++)
		{
			m_Keybindings[i].button->Tick(elapsed);
			if (m_Keybindings[i].button->IsDown())
			{
				m_KeybindingAssigningIndex = i;
				return;
			}
		}
	} break;
	}
}

void ApexPauseScreen::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	const sf::Vector2f renderTargetSize = static_cast<sf::Vector2f>(target.getSize());
	sf::RectangleShape bgRect(renderTargetSize);
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

		m_KeybindingsButton->Draw(target, states);
	} break;
	case Screen::KEYBINDINGS:
	{
		for (size_t i = 0; i < m_Keybindings.size(); i++)
		{
			target.draw(m_Keybindings[i].label, states);
			m_Keybindings[i].button->Draw(target, states);
		}

		if (m_KeybindingAssigningIndex != -1)
		{
			bgRect.setFillColor(sf::Color(25, 25, 30, 100));
			target.draw(bgRect);

			sf::RectangleShape centerRect(sf::Vector2f(200, 100));
			centerRect.setPosition(renderTargetSize.x / 2.0f - 50, renderTargetSize.y / 2.0f);
			centerRect.setFillColor(sf::Color(165, 230, 130));
			target.draw(centerRect, states);

			sf::Text text("Press any key to assign to " + m_Keybindings[m_KeybindingAssigningIndex].key, APEX->FontPixelFJ8);
			text.setPosition(renderTargetSize.x / 2.0f - 20, renderTargetSize.y / 2.0f + 10);
			target.draw(text, states);
		}
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
	m_KeybindingsButton->ClearInputs();

	for (size_t i = 0; i < m_Keybindings.size(); i++)
	{
		m_Keybindings[i].button->ClearInputs();
	}
}

bool ApexPauseScreen::OnKeyPress(ApexKeyboard::Key key, bool keyPressed)
{
	if (keyPressed)
	{
		switch (m_CurrentScreenShowing)
		{
		case Screen::MAIN:
		{
			switch (key)
			{
			case ApexKeyboard::PAUSE:
			{
				m_Level->TogglePaused(true);
				return true;
			} break;
			}
		} break;
		case Screen::OPTIONS:
		{
			switch (key)
			{
			case ApexKeyboard::PAUSE:
			{
				SetScreenShowing(Screen::MAIN);
				return true;
			} break;
			}
		} break;
		case Screen::KEYBINDINGS:
		{
			if (m_KeybindingAssigningIndex != -1)
			{
				int vkCode;
				if (ApexKeyboard::GetUnMappedKey(key, vkCode))
				{
					for (size_t i = 0; i < m_Keybindings.size(); i++)
					{
						if (m_Keybindings[i].key == key)
						{
							AssignKeybinding(m_Keybindings[i], vkCode);
							return true;
						}
					}
				}
			}
			else
			{
				switch (key)
				{
				case ApexKeyboard::PAUSE:
				{
					m_KeybindingAssigningIndex = -1;
					SetScreenShowing(Screen::OPTIONS);
					return true;
				} break;
				}
			}
		} break;
		}
	}
	return false;
}

void ApexPauseScreen::OnUnmappedKeyPress(sf::Event::KeyEvent event)
{
	if (m_KeybindingAssigningIndex != -1)
	{
		AssignKeybinding(m_Keybindings[m_KeybindingAssigningIndex], event.code);
	}
}

void ApexPauseScreen::OnKeyRelease(ApexKeyboard::Key key)
{
}

#include "windows.h"
void ApexPauseScreen::AssignKeybinding(Keybinding& keybinding, int vkCode)
{
	ApexKeyboard::MapKey(keybinding.key, vkCode);

	keybinding.button->SetString(0, ApexKeyboard::GetSFKeyName(sf::Keyboard::Key(vkCode)));
	m_KeybindingAssigningIndex = -1;
}
