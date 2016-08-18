
#include "ApexPauseScreen.h"
#include "World.h"
#include "GameState.h"
#include "ApexMain.h"
#include "MainMenuState.h"
#include "ApexMouse.h"
#include "ApexButton.h"
#include "ApexSlider.h"
#include "ApexAudio.h"

#include <SFML\Graphics\RectangleShape.hpp>

ApexPauseScreen::ApexPauseScreen(World* world) :
	ApexKeyListener(),
	m_World(world)
{
	float width = 300.0f;
	float height = 120.0f;
	sf::Vector2u windowSize = APEX->GetWindowSize();
	float centerX = windowSize.x / 2.0f;
	float left = centerX - width / 2.0f;
	float initialTop = 550.0f;
	float top = initialTop;
	float dy = height * 1.5f;

	const unsigned int fontSize = 38;
	const sf::Font& font = APEX->FontPixelFJ8;

	// Main screen
	ApexButton* resumeButton = new ApexButton(left, top, width, height, "Resume");
	m_MainScreenButtonList.AddButton(resumeButton, int(MainScreenButtons::RESUME));
	top += dy;
	ApexButton* optionsButton = new ApexButton(left, top, width, height, "Options");
	m_MainScreenButtonList.AddButton(optionsButton, int(MainScreenButtons::OPTIONS));
	top += dy;
	ApexButton* mainMenuButton = new ApexButton(left, top, width, height, "Main Menu");
	m_MainScreenButtonList.AddButton(mainMenuButton, int(MainScreenButtons::MAIN_MENU));

	// Options screen
	top = initialTop;
	ApexButton* fullscreenButton = new ApexButton(left, top, width, height, "Fullscreen");
	fullscreenButton->AddString("Windowed");
	m_OptionsButtonList.AddButton(fullscreenButton, int(OptionsScreenButtons::FULLSCREEN));

	top += dy;
	const float bindingsButtonWidth = width * 2.5f;
	ApexButton* keybindingsButton = new ApexButton(left + (width * 0.5f - bindingsButtonWidth * 0.5f), top, bindingsButtonWidth, height, "Keyboard bindings");
	m_OptionsButtonList.AddButton(keybindingsButton, int(OptionsScreenButtons::KEYBINDINGS));

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
	const int buttonXO = 750;
	const float smallButtonWidth = 160.0f;
	const float smallButtonHeight = 70.0f;
	const float labelYO = 25.0f;
	const float maxY = windowSize.y - smallButtonHeight * 2.0f;
	dy = smallButtonHeight * 1.5f;
	initialTop = 90.0f;
	top = initialTop;
	left = 100.0f;
	for (size_t i = 0; i < ApexKeyboard::GetNumberOfKeys(); i++)
	{
		Keybinding keybinding;

		keybinding.label = sf::Text(ApexKeyboard::GetKeyName(ApexKeyboard::Key(i)), font, fontSize);
		keybinding.label.setPosition(left, top + labelYO);
		keybinding.key = ApexKeyboard::Key(i);
		ApexKeyboard::GetVKCodeFromKey(keybinding.key, vkCode);
		keybinding.button = new ApexButton(left + buttonXO, top, smallButtonWidth, smallButtonHeight, 
			ApexKeyboard::GetSFKeyName(sf::Keyboard::Key(vkCode)), 42);
		
		m_Keybindings.push_back(keybinding);

		top += dy;

		if (top > maxY) 
		{
			top = initialTop;
			left += 975.0f;
		}
	}

	m_KeybindingAssigningIndex = -1;

	m_CurrentScreenShowing = Screen::MAIN;
}

ApexPauseScreen::~ApexPauseScreen()
{
	delete m_MusicVolumeSlider;
	delete m_SoundVolumeSlider;

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
		m_MainScreenButtonList.Tick(elapsed);
		if (m_MainScreenButtonList.GetButton(int(MainScreenButtons::RESUME))->IsPressed())
		{
			m_World->TogglePaused(true);
			return;
		}

		if (m_MainScreenButtonList.GetButton(int(MainScreenButtons::MAIN_MENU))->IsPressed())
		{
			APEX->GetStateManager()->SetState(new MainMenuState());
			return;
		}

		if (m_MainScreenButtonList.GetButton(int(MainScreenButtons::OPTIONS))->IsPressed())
		{
			SetScreenShowing(Screen::OPTIONS);
			return;
		}
	} break;
	case Screen::OPTIONS:
	{
		m_OptionsButtonList.Tick(elapsed);
		if (m_OptionsButtonList.GetButton(int(OptionsScreenButtons::FULLSCREEN))->IsPressed())
		{
			m_OptionsButtonList.GetButton(int(OptionsScreenButtons::FULLSCREEN))->ClearInputs();
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

		if (m_OptionsButtonList.GetButton(int(OptionsScreenButtons::KEYBINDINGS))->IsPressed())
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
		m_MainScreenButtonList.Draw(target, states);
	} break;
	case Screen::OPTIONS:
	{
		m_OptionsButtonList.Draw(target, states);

		target.draw(m_MusicVolumeText, states);
		m_MusicVolumeSlider->Draw(target, states);

		target.draw(m_SoundVolumeText, states);
		m_SoundVolumeSlider->Draw(target, states);
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

			sf::RectangleShape centerRect(sf::Vector2f(500, 145));
			centerRect.setPosition(renderTargetSize.x / 2.0f - 250, renderTargetSize.y / 2.0f);
			centerRect.setFillColor(sf::Color(165, 230, 130));
			target.draw(centerRect, states);

			int vkCode;
			ApexKeyboard::GetVKCodeFromKey(ApexKeyboard::PAUSE, vkCode);
			sf::Text text("Press any key to\nassign to " + ApexKeyboard::GetKeyName(m_Keybindings[m_KeybindingAssigningIndex].key) + 
			"\n(" + ApexKeyboard::GetSFKeyName(sf::Keyboard::Key(vkCode)) + " to cancel)", APEX->FontPixelFJ8);
			text.setPosition(renderTargetSize.x / 2.0f - 240, renderTargetSize.y / 2.0f + 20);
			text.setColor(sf::Color::Black);
			target.draw(text, states);
		}
	} break;
	}
}

void ApexPauseScreen::SetScreenShowing(Screen screen)
{
	m_CurrentScreenShowing = screen;

	m_MainScreenButtonList.GetButton(int(MainScreenButtons::RESUME))->ClearInputs();
	m_MainScreenButtonList.GetButton(int(MainScreenButtons::OPTIONS))->ClearInputs();
	m_MainScreenButtonList.GetButton(int(MainScreenButtons::MAIN_MENU))->ClearInputs();
	m_OptionsButtonList.GetButton(int(OptionsScreenButtons::FULLSCREEN))->ClearInputs();
	m_OptionsButtonList.GetButton(int(OptionsScreenButtons::KEYBINDINGS))->ClearInputs();

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
				m_World->TogglePaused(true);
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
				if (key == ApexKeyboard::PAUSE)
				{
					m_KeybindingAssigningIndex = -1;
				}
				else
				{
					int vkCode;
					if (ApexKeyboard::GetVKCodeFromKey(key, vkCode))
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

void ApexPauseScreen::AssignKeybinding(Keybinding& keybinding, int vkCode)
{
	ApexKeyboard::MapKey(keybinding.key, vkCode);

	keybinding.button->SetString(0, ApexKeyboard::GetSFKeyName(sf::Keyboard::Key(vkCode)));
	m_KeybindingAssigningIndex = -1;
}