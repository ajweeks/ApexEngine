
#include "Game.h"
#include "GameState.h"
#include "enumerations.h"
#include "ApexKeyboard.h"
#include "ApexMouse.h"
#include "ApexDebug.h"
#include <iostream>
#include <Windows.h>
#include <sstream>
#include <iomanip>

const int Game::INITAL_WINDOW_WIDTH = 2160;
const int Game::INITAL_WINDOW_HEIGHT = 1215;
const bool Game::USE_V_SYNC = true;
const sf::String Game::WINDOW_TITLE = "Apex Engine";

sf::Font Game::font12;

Game::Game() :
	m_StateManager(new GameState(&m_StateManager, this))
{
	m_Window.create(sf::VideoMode(INITAL_WINDOW_WIDTH, INITAL_WINDOW_HEIGHT), WINDOW_TITLE);
	m_Window.setVerticalSyncEnabled(USE_V_SYNC);

	if (!font12.loadFromFile("resources/OpenSans/OpenSans-Regular.ttf"))
	{
		std::cout << "Couldn't load font OpenSans-Regular.ttf!" << std::endl;
	}

	IntializeDebug();
}

Game::~Game()
{
	delete m_Debug;
}

void Game::IntializeDebug()
{
	m_Debug = new ApexDebug();
	CollapsibleElement* parentElement = m_Debug->CreateCollapsibleElementStack("Parent1", sf::Vector2f(700, 400));
	CollapsibleElement* child1 = m_Debug->AddCollapsibleElementChild(parentElement, "child1");
	CollapsibleElement* child1_1 = m_Debug->AddCollapsibleElementChild(child1, "child1_1");
	m_Debug->AddCollapsibleElementChild(child1_1, "child1_1_1");
	m_Debug->AddCollapsibleElementChild(parentElement, "child2");
	CollapsibleElement* child3 = m_Debug->AddCollapsibleElementChild(parentElement, "child3");
	m_Debug->AddCollapsibleElementChild(child3, "child3_1");
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

		ApexKeyboard::Tick();
		ApexMouse::Tick();

		sf::Event event;
		while (m_Window.pollEvent(event))
		{
			switch(event.type)
			{
			case sf::Event::Closed:
				running = false;
				break;
			case sf::Event::Resized:
				// Uncomment to prevent window scale from scaling content
				//sf::FloatRect visibleArea(0, 0, float(event.size.width), float(event.size.height));
				//m_View.reset(visibleArea);
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code)
				{
				case sf::Keyboard::F10:
				{
					// Prevent multiple calls from a press and hold
					if (ApexKeyboard::IsKeyPressed(sf::Keyboard::F9))
					{
						TakeScreenshot();
					}
				}
				case sf::Keyboard::F5:
				{
					// Prevent multiple calls from a press and hold
					if (ApexKeyboard::IsKeyPressed(sf::Keyboard::F5))
					{
						m_ShowDebug = !m_ShowDebug;
					}
				}
				}
				break;
			}
		}

		Tick(elapsed);
		Draw();
		++m_Frames;
	}
}

void Game::Tick(sf::Time elapsed)
{
	m_StateManager.Tick(elapsed);
	if (m_ShowDebug) m_Debug->Tick(elapsed, this);
}

void Game::Draw()
{
	m_Window.clear();
	
	m_StateManager.Draw(m_Window);
	if (m_ShowDebug) m_Debug->Draw(m_Window);
	
	m_Window.display();
}

sf::Vector2f Game::MapPixelToCoords(sf::Vector2i pixels) const
{
	return m_Window.mapPixelToCoords(pixels);
}

sf::Vector2f Game::GetMouseCoordsWorldSpace() const
{
	sf::Vector2i mouse = sf::Mouse::getPosition(m_Window);
	return m_Window.mapPixelToCoords(mouse);
}

sf::Vector2i Game::GetMouseCoordsScreenSpace() const
{
	return  sf::Mouse::getPosition(m_Window);
}

void Game::SetCursor(sf::StandardCursor::TYPE cursorType)
{
	sf::StandardCursor Cursor(cursorType);
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
