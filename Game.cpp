
#include "Game.h"
#include "GameState.h"
#include "enumerations.h"
#include "ApexKeyboard.h"
#include "KeyListener.h"
#include "ApexMouse.h"
#include "ApexDebug.h"
#include "ApexAudio.h"
#include "Entity.h"
#include "StateManager.h"
#include "logo.h"

#include <iostream>
#include <sstream>
#include <iomanip>

const int Game::INITAL_WINDOW_WIDTH = 2080;
const int Game::INITAL_WINDOW_HEIGHT = 1216;
const bool Game::USE_V_SYNC = false;
const std::string Game::WINDOW_TITLE = "Apex Engine";

sf::Font Game::font12;

Game::Game()
{
	m_Window.create(sf::VideoMode(INITAL_WINDOW_WIDTH, INITAL_WINDOW_HEIGHT), WINDOW_TITLE);
	m_Window.setVerticalSyncEnabled(USE_V_SYNC);
	m_Window.setIcon(apex_logo.width, apex_logo.height, apex_logo.pixel_data);
	
	m_StateManager = new StateManager(new GameState(m_StateManager, this));

	if (!font12.loadFromFile("resources/font/OpenSans/OpenSans-Regular.ttf"))
	{
		std::cout << "Couldn't load font OpenSans-Regular.ttf!" << std::endl;
	}

	ApexAudio::LoadSounds();
}

Game::~Game()
{
	delete m_StateManager;
}
void Game::Run()
{
	sf::Clock clock;

	bool running = true;
	while (running)
	{
		sf::Time elapsed = clock.restart();
		m_ElapsedThisFrame += elapsed;
		if (m_ElapsedThisFrame.asSeconds() >= 1.0f)
		{
			m_ElapsedThisFrame -= sf::seconds(1.0f);
			m_FPS = m_Frames;
			m_Window.setTitle(WINDOW_TITLE + " - " + std::to_string(m_FPS) + " fps");
			m_Frames = 0;
		}

		if (m_Window.hasFocus())
		{
			ApexKeyboard::Tick();
			ApexMouse::Tick();
		}

		sf::Event event;
		while (m_Window.pollEvent(event))
		{
			switch(event.type)
			{
			case sf::Event::Closed:
			{
				running = false;
				break;
			}
			case sf::Event::LostFocus:
			{
				ApexAudio::SetAllPaused(true);
				break;
			}
			case sf::Event::GainedFocus:
			{
				ApexAudio::SetAllPaused(false);
				break;
			}
			case sf::Event::KeyPressed:
			{
				if (m_Window.hasFocus())
				{
					for (size_t i = 0; i < m_KeyListeners.size(); ++i)
					{
						if (m_KeyListeners[i] != nullptr)
						{
							m_KeyListeners[i]->OnKeyPress(event.key);
						}
					}

					switch (event.key.code)
					{
					case sf::Keyboard::F10:
					{
						// Prevent multiple calls from a press and hold
						if (ApexKeyboard::IsKeyPressed(event.key.code))
						{
							TakeScreenshot();
						}
					} break;
					}
				}
			} break;
			case sf::Event::KeyReleased:
			{
				if (m_Window.hasFocus())
				{
					for (size_t i = 0; i < m_KeyListeners.size(); ++i)
					{
						if (m_KeyListeners[i] != nullptr)
						{
							m_KeyListeners[i]->OnKeyRelease(event.key);
						}
					}
				}
			} break;
			}
		}

		Tick(elapsed);
		Draw();
		++m_Frames;
	}
}

void Game::Tick(sf::Time elapsed)
{
	m_StateManager->Tick(elapsed);
}

void Game::Draw()
{
	m_Window.clear();
	
	m_StateManager->Draw(m_Window);

	m_Window.setView(m_Window.getDefaultView());
	
	m_Window.display();
}

sf::Vector2f Game::GetMouseCoordsWorldSpace(sf::View view) const
{
	sf::Vector2i mouse = sf::Mouse::getPosition(m_Window);
	return m_Window.mapPixelToCoords(mouse, view);
}

sf::Vector2i Game::GetMouseCoordsScreenSpace(sf::View currentView) const
{
	sf::Vector2i mouseCoords = sf::Mouse::getPosition(m_Window);
	if (m_Window.getSize() != static_cast<sf::Vector2u>(currentView.getSize()))
	{
		// Adjust for when the window has been resized the content stretched or squeezed
		const float xScale = currentView.getSize().x / m_Window.getSize().x;
		const float yScale = currentView.getSize().y / m_Window.getSize().y;
		mouseCoords.x = static_cast<int>(mouseCoords.x * xScale);
		mouseCoords.y = static_cast<int>(mouseCoords.y * yScale);
	}
	return mouseCoords;
}

void Game::SetCursor(sf::ApexCursor::TYPE cursorType)
{
	sf::ApexCursor Cursor(cursorType);
	Cursor.set(m_Window.getSystemHandle());
}

void Game::TakeScreenshot()
{
	sf::Image screen = m_Window.capture();
	SYSTEMTIME localTime;
	GetLocalTime(&localTime);

	std::stringstream filenameStream;
	filenameStream << std::to_string(localTime.wYear) << "-";
	filenameStream << std::setw(2) << std::setfill('0') << std::to_string(localTime.wMonth) << "-";
	filenameStream << std::setw(2) << std::setfill('0') << std::to_string(localTime.wDay);
	const std::string dateString = filenameStream.str();

	sf::FileInputStream fileInStream;
	int index = 0;
	// Keep opening files until one doesn't exist
	while (fileInStream.open("screenshots/" + dateString + "_" + std::to_string(index) + ".png"))
		++index;

	const std::string filename = "screenshots/" + dateString + "_" + std::to_string(index) + ".png";
	screen.saveToFile(filename);

	std::cout << "Saved screenshot as \"" + filename << "\" successfully!" << std::endl;
}

StateManager* Game::GetStateManager()
{
	return m_StateManager;
}

sf::Vector2u Game::GetWindowSize() const
{
	return m_Window.getSize();
}

std::string Game::Vector2fToString(sf::Vector2f vec)
{
	std::stringstream stream;
	stream  << std::setprecision(2) << std::fixed << vec.x << "," << vec.y;
	return stream.str();
}

void Game::AddKeyListener(KeyListener* keyListener)
{
	for (size_t i = 0; i < m_KeyListeners.size(); ++i)
	{
		if (m_KeyListeners[i] == nullptr) 
		{
			m_KeyListeners[i] = keyListener;
			return;
		}
	}
	m_KeyListeners.push_back(keyListener);
}

void Game::RemoveKeyListener(KeyListener * keyListener)
{
	for (size_t i = 0; i < m_KeyListeners.size(); ++i)
	{
		if (m_KeyListeners[i] == keyListener) m_KeyListeners[i] = nullptr;
	}
}
