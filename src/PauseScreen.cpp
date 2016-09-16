
#include "PauseScreen.h"
#include "World.h"
#include "GameState.h"
#include "ApexMain.h"
#include "MainMenuState.h"
#include "ApexMouse.h"
#include "ApexButton.h"
#include "ApexSlider.h"
#include "ApexAudio.h"
#include "Map.h"

#include <SFML\Graphics\RectangleShape.hpp>

#include <JSON\json.hpp>

#include <fstream>

using nlohmann::json;

PauseScreen::PauseScreen(World& world) :
	ApexKeyListener(0),
	m_World(world)
{
	ReadOptionsFromFile();

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
	ApexButton* resumeButton = new ApexButton(left, top, width, height, "Resume", fontSize);
	m_MainScreenButtonList.AddButton(resumeButton, int(MainScreenButtons::RESUME));
	top += dy;
	ApexButton* optionsButton = new ApexButton(left, top, width, height, "Options", fontSize);
	m_MainScreenButtonList.AddButton(optionsButton, int(MainScreenButtons::OPTIONS));
	top += dy;
	ApexButton* mainMenuButton = new ApexButton(left, top, width, height, "Main Menu", fontSize);
	m_MainScreenButtonList.AddButton(mainMenuButton, int(MainScreenButtons::MAIN_MENU));

	// Options screen
	initialTop = 250.0f;
	top = initialTop;
	ApexButton* fullscreenButton = new ApexButton(left, top, width, height, "Fullscreen", fontSize);
	fullscreenButton->AddString("Windowed");
	if (!m_Options.fullscreen)
	{
		fullscreenButton->ShowNextString();
	}
	m_OptionsButtonList.AddButton(fullscreenButton, int(OptionsScreenButtons::FULLSCREEN));

	top += dy;
	ApexButton* enableVSyncButton = new ApexButton(left, top, width, height, "Use V-Sync: Off", fontSize);
	enableVSyncButton->AddString("Use V-Sync: On");
	if (m_Options.useVSync)
	{
		enableVSyncButton->ShowNextString();
	}
	m_OptionsButtonList.AddButton(enableVSyncButton, int(OptionsScreenButtons::ENABLE_V_SYNC));

	top += dy;
	ApexButton* renderLightingButton = new ApexButton(left, top, width, height, "Lighting: Off", fontSize);
	renderLightingButton->AddString("Lighting: On");
	if (m_Options.renderLighting)
	{
		renderLightingButton->ShowNextString();
	}
	m_OptionsButtonList.AddButton(renderLightingButton, int(OptionsScreenButtons::RENDER_LIGHTING));

	top += dy;
	const float bindingsButtonWidth = width * 2.5f;
	ApexButton* keybindingsButton = new ApexButton(left + (width * 0.5f - bindingsButtonWidth * 0.5f), top, 
		bindingsButtonWidth, height, "Keyboard bindings", fontSize);
	m_OptionsButtonList.AddButton(keybindingsButton, int(OptionsScreenButtons::KEYBINDINGS));

	top += dy;
	m_MusicVolumeText = sf::Text("Music Volume:", font, fontSize);
	m_MusicVolumeText.setFillColor(sf::Color(210, 210, 210));
	m_MusicVolumeText.setPosition(left, top);
	
	const float volumeBarWidth = width * 1.2f;
	const float volumeBarHeight = height * 0.25f;
	top += height * 0.5f;
	m_MusicVolumeSlider = new ApexSlider(volumeBarWidth, volumeBarHeight, centerX - volumeBarWidth / 2, top);
	m_MusicVolumeSlider->SetValue(m_Options.musicVolume);

	top += height * 0.8f;
	m_SoundVolumeText = sf::Text("Sound Volume:", font, fontSize);
	m_SoundVolumeText.setFillColor(sf::Color(210, 210, 210));
	m_SoundVolumeText.setPosition(left, top);

	top += height * 0.5f;
	m_SoundVolumeSlider = new ApexSlider(volumeBarWidth, volumeBarHeight, centerX - volumeBarWidth / 2, top);
	m_SoundVolumeSlider->SetValue(m_Options.soundVolume);

	// Keybindings screen
	int vkCode;
	const int buttonXO = 750;
	const float smallButtonWidth = 160.0f;
	const float smallButtonHeight = 70.0f;
	const float labelYO = 25.0f;
	const float maxY = windowSize.y - smallButtonHeight * 2.0f;
	dy = smallButtonHeight * 1.5f;
	initialTop = smallButtonHeight;
	top = initialTop;
	left = 100.0f;
	const size_t numOfKeys = ApexKeyboard::GetNumberOfKeys();
	for (size_t i = 0; i < numOfKeys; i++)
	{
		Keybinding keybinding;

		keybinding.label = sf::Text(ApexKeyboard::GetKeyName(ApexKeyboard::Key(i)), font, fontSize);
		keybinding.label.setPosition(left, top + labelYO);
		keybinding.key = ApexKeyboard::Key(i);
		ApexKeyboard::GetVKCodeFromKey(keybinding.key, vkCode);
		keybinding.button = new ApexButton(left + buttonXO, top, smallButtonWidth, smallButtonHeight, 
			ApexKeyboard::GetSFKeyName(sf::Keyboard::Key(vkCode)), 42);
		
		m_Options.keybindings.push_back(keybinding);

		top += dy;

		if (top > maxY) 
		{
			top = initialTop;
			left += 975.0f;
		}
	}

	m_KeybindingAssigningIndex = -1;

	SetScreenShowing(Screen::NONE);
}

PauseScreen::~PauseScreen()
{
	delete m_MusicVolumeSlider;
	delete m_SoundVolumeSlider;
	
	for (size_t i = 0; i < m_Options.keybindings.size(); i++)
	{
		delete m_Options.keybindings[i].button;
	}
	m_Options.keybindings.clear();
}

void PauseScreen::Tick(sf::Time elapsed)
{
	switch (m_CurrentScreenShowing)
	{
	case Screen::MAIN:
	{
		m_MainScreenButtonList.Tick(elapsed);
		if (m_MainScreenButtonList.GetButton(int(MainScreenButtons::RESUME))->IsPressed())
		{
			m_World.TogglePaused(true);
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
			APEX->ToggleWindowFullscreen();
			m_Options.fullscreen = APEX->IsWindowFullscreen();
			return;
		}
		if (m_OptionsButtonList.GetButton(int(OptionsScreenButtons::ENABLE_V_SYNC))->IsPressed())
		{
			m_OptionsButtonList.GetButton(int(OptionsScreenButtons::ENABLE_V_SYNC))->ClearInputs();
			ApexMouse::Clear();
			APEX->ToggleVSyncEnabled();
			m_Options.useVSync = APEX->IsVSyncEnabled();
			return;
		}
		if (m_OptionsButtonList.GetButton(int(OptionsScreenButtons::RENDER_LIGHTING))->IsPressed())
		{
			m_OptionsButtonList.GetButton(int(OptionsScreenButtons::RENDER_LIGHTING))->ClearInputs();
			ApexMouse::Clear();
			m_World.ToggleLightingEnabled();
			m_Options.renderLighting = m_World.IsLightingEnabled();
			return;
		}

		const float prevMusicVolumeValue = m_MusicVolumeSlider->GetValue();
		m_MusicVolumeSlider->Tick(elapsed);
		const float newMusicVolumeValue = m_MusicVolumeSlider->GetValue();
		if (prevMusicVolumeValue != newMusicVolumeValue)
		{
			ApexAudio::SetAllMusicVolume(newMusicVolumeValue);
			m_Options.musicVolume = newMusicVolumeValue;
		}

		const float prevSoundVolumeValue = m_SoundVolumeSlider->GetValue();
		m_SoundVolumeSlider->Tick(elapsed);
		const float newSoundVolumeValue = m_SoundVolumeSlider->GetValue();
		if (prevSoundVolumeValue != newSoundVolumeValue)
		{
			ApexAudio::SetAllSoundsVolume(newSoundVolumeValue);
			m_Options.soundVolume = newSoundVolumeValue;
		}
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
		for (size_t i = 0; i < m_Options.keybindings.size(); i++)
		{
			m_Options.keybindings[i].button->Tick(elapsed);
			if (m_Options.keybindings[i].button->IsDown())
			{
				m_KeybindingAssigningIndex = i;
				return;
			}
		}
	} break;
	}
}

void PauseScreen::Draw(sf::RenderTarget& target, sf::RenderStates states)
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
		for (size_t i = 0; i < m_Options.keybindings.size(); i++)
		{
			target.draw(m_Options.keybindings[i].label, states);
			m_Options.keybindings[i].button->Draw(target, states);
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
			sf::Text text("Press any key to\nassign to " + ApexKeyboard::GetKeyName(m_Options.keybindings[m_KeybindingAssigningIndex].key) +
			"\n(" + ApexKeyboard::GetSFKeyName(sf::Keyboard::Key(vkCode)) + " to cancel)", APEX->FontPixelFJ8);
			text.setPosition(renderTargetSize.x / 2.0f - 240, renderTargetSize.y / 2.0f + 20);
			text.setFillColor(sf::Color::Black);
			target.draw(text, states);
		}
	} break;
	}
}

void PauseScreen::SetScreenShowing(Screen screen)
{
	m_CurrentScreenShowing = screen;
	m_MainScreenButtonList.SetActive(screen == Screen::MAIN);
	m_OptionsButtonList.SetActive(screen == Screen::OPTIONS);

	for (size_t i = 0; i < int(MainScreenButtons::_LAST_ELEMENT); i++)
	{
		m_MainScreenButtonList.GetButton(i)->ClearInputs();
	}
	for (size_t i = 0; i < int(OptionsScreenButtons::_LAST_ELEMENT); i++)
	{
		m_OptionsButtonList.GetButton(i)->ClearInputs();
	}

	for (size_t i = 0; i < m_Options.keybindings.size(); i++)
	{
		m_Options.keybindings[i].button->ClearInputs();
	}
}

bool PauseScreen::OnKeyPress(ApexKeyboard::Key key, bool keyPressed)
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
				m_World.TogglePaused(true);
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
				SaveOptionsToFile();
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
						for (size_t i = 0; i < m_Options.keybindings.size(); i++)
						{
							if (m_Options.keybindings[i].key == key)
							{
								AssignKeybinding(m_Options.keybindings[i], vkCode);
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
		case Screen::NONE:
		default:
		{
		} break;
		}
	}
	return false;
}

void PauseScreen::OnUnmappedKeyPress(sf::Event::KeyEvent event)
{
	if (m_KeybindingAssigningIndex != -1)
	{
		AssignKeybinding(m_Options.keybindings[m_KeybindingAssigningIndex], event.code);
	}
}

void PauseScreen::OnKeyRelease(ApexKeyboard::Key key)
{
}

void PauseScreen::AssignKeybinding(Keybinding& keybinding, int vkCode)
{
	ApexKeyboard::MapKey(keybinding.key, vkCode);

	keybinding.button->SetString(0, ApexKeyboard::GetSFKeyName(sf::Keyboard::Key(vkCode)));
	m_KeybindingAssigningIndex = -1;
}

void PauseScreen::SaveOptionsToFile()
{
	std::ofstream fileOutStream;
	const std::string filePath = "resources/options.json";
	std::stringstream stringStream;

	fileOutStream.open(filePath);

	if (fileOutStream)
	{
		json fileJson;

		// TODO: Store fewer floating point digits
		fileJson["options"]["fullscreen"] = m_Options.fullscreen;
		fileJson["options"]["useVSync"] = m_Options.useVSync;
		fileJson["options"]["renderLighting"] = m_Options.renderLighting;
		fileJson["options"]["musicVolume"] = m_Options.musicVolume;
		fileJson["options"]["soundVolume"] = m_Options.soundVolume;
		
		fileOutStream << std::setw(4) << fileJson;
		fileOutStream.close();
	}
}

template<typename T>
T PauseScreen::FindOption(const json& options, const std::string& option)
{
	if (options.find(option) != options.end())
	{
		return options[option].get<T>();
	}
	return T();
}
void PauseScreen::ReadOptionsFromFile()
{
	const std::string filePath = "resources/options.json";
	std::ifstream fileInStream;
	std::stringstream stringStream;

	fileInStream.open(filePath);

	if (fileInStream)
	{
		std::string line;
		while (fileInStream.eof() == false)
		{
			std::getline(fileInStream, line);
			stringStream << line;
		}
		fileInStream.close();

		if (!stringStream.str().empty())
		{
			json fileJson = json::parse(stringStream.str());

			m_Options = {};
			m_Options.fullscreen = FindOption<bool>(fileJson["options"], "fullscreen");
			m_Options.useVSync = FindOption<bool>(fileJson["options"], "useVSync");
			m_Options.renderLighting = FindOption<bool>(fileJson["options"], "renderLighting");
			m_Options.musicVolume = FindOption<float>(fileJson["options"], "musicVolume");
			m_Options.soundVolume = FindOption<float>(fileJson["options"], "soundVolume");

			APEX->SetWindowFullscreen(m_Options.fullscreen);
			APEX->SetVSyncEnabled(m_Options.useVSync);
			m_World.SetLightingEnabled(m_Options.renderLighting);
			ApexAudio::SetAllSoundsVolume(m_Options.soundVolume);
			ApexAudio::SetAllMusicVolume(m_Options.musicVolume);
		}
	}
}

