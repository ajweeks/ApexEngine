
#include "ApexMain.h"
#include "GameState.h"
#include "MainMenuState.h"
#include "enumerations.h"
#include "ApexKeyboard.h"
#include "ApexKeyListener.h"
#include "ApexMouse.h"
#include "ApexMouseListener.h"
#include "ApexDebug.h"
#include "ApexAudio.h"
#include "Entity.h"
#include "StateManager.h"
#include "logo.h"
#include "PhysicsActorManager.h"

#include <windows.h> // ugh (only required for OutputDebugString I think)
#include <sstream>
#include <iomanip>

const int ApexMain::INITAL_WINDOW_WIDTH = 2080;
const int ApexMain::INITAL_WINDOW_HEIGHT = 1216;
const bool ApexMain::USE_V_SYNC = false;
const std::string ApexMain::WINDOW_TITLE = "Apex Engine";

ApexMain* ApexMain::m_Singleton = nullptr;
sf::Font ApexMain::FontOpenSans;

void ApexOutputDebugString(const std::string& string)
{
	OutputDebugStringA(string.c_str());
}

ApexMain::ApexMain()
{
	if (!sf::Shader::isAvailable())
	{
		throw std::exception("Sorry, your graphics card doesn't support shaders :(");
	}
}

ApexMain::~ApexMain()
{
	delete m_StateManager;
	delete m_PhysicsActorManager;
	delete m_Window;
}

void ApexMain::Init()
{
	m_WindowIsFullscreen = false;
	CreateApexWindow(m_WindowIsFullscreen);
	LoadCursorTextures();

	m_PhysicsActorManager = new PhysicsActorManager(*m_Window);

	m_StateManager = new StateManager(new MainMenuState());

	if (!FontOpenSans.loadFromFile("resources/font/OpenSans/OpenSans-Regular.ttf"))
	{
		ApexOutputDebugString("Couldn't load font OpenSans-Regular.ttf!\n");
	}

	ApexAudio::LoadSounds();
}

void ApexMain::CreateApexWindow(bool fullscreen)
{
	delete m_Window;
	m_Window = new sf::RenderWindow(sf::VideoMode(INITAL_WINDOW_WIDTH, INITAL_WINDOW_HEIGHT), WINDOW_TITLE, (fullscreen ? sf::Style::Fullscreen : sf::Style::Close));
	m_Window->setVerticalSyncEnabled(USE_V_SYNC);
	m_Window->setIcon(apex_logo.width, apex_logo.height, apex_logo.pixel_data);
	m_Window->setMouseCursorVisible(false);
}

void ApexMain::Quit()
{
	m_IsRunning = false;
}

void ApexMain::Run()
{
	sf::Clock clock;

	double accumulator = 0.0f;
	m_IsRunning = true;
	while (m_IsRunning)
	{
		sf::Time elapsed = clock.restart();

		// Calculate FPS
		m_ElapsedThisFrame += elapsed;
		m_TotalElapsed += elapsed;
		if (m_ElapsedThisFrame.asSeconds() >= 1.0f)
		{
			m_ElapsedThisFrame -= sf::seconds(1.0f);
			m_FPS = m_Frames;
			m_Window->setTitle(WINDOW_TITLE + " - " + std::to_string(m_FPS) + " fps");
			m_Frames = 0;
		}
		
		if (m_Window->hasFocus())
		{
			ApexKeyboard::Tick();
			ApexMouse::Tick();
		}

		bool stepOneFrame = false;

		// Process events
		sf::Event event;
		while (m_Window->pollEvent(event))
		{
			if (event.type == sf::Event::LostFocus)
			{
				ApexAudio::SetAllPaused(true);
				ApexKeyboard::Clear();
				ApexMouse::Clear();
			}

			if (m_Window->hasFocus())
			{
				switch (event.type)
				{
				case sf::Event::Closed:
				{
					m_IsRunning = false;
				} break;
				case sf::Event::GainedFocus:
				{
					ApexAudio::SetAllPaused(false);
				} break;
				case sf::Event::KeyPressed:
				{
					const bool keyPressed = ApexKeyboard::IsKeyPressed(event.key.code);
					for (size_t i = 0; i < m_KeyListeners.size(); ++i)
					{
						if (m_KeyListeners[i] != nullptr)
						{
							if (m_KeyListeners[i]->OnKeyPress(event.key, keyPressed))
							{
								break;
							}
						}
					}

					if (keyPressed)
					{
						switch (event.key.code)
						{
						case sf::Keyboard::F10:
						{
							TakeScreenshot();
						} break;
						case sf::Keyboard::Space:
						{
							DEBUGToggleGamePaused();
						} break;
						case sf::Keyboard::Period:
						{
							if (m_DEBUG_GamePaused) stepOneFrame = true;
						} break;
						case sf::Keyboard::P:
						{
							m_ShowingPhysicsDebug = !m_ShowingPhysicsDebug;
						} break;
						}
					}
				} break;
				case sf::Event::KeyReleased:
				{
					for (size_t i = 0; i < m_KeyListeners.size(); ++i)
					{
						if (m_KeyListeners[i] != nullptr)
						{
							m_KeyListeners[i]->OnKeyRelease(event.key);
						}
					}
				} break;
				case sf::Event::MouseButtonPressed:
				{
					for (size_t i = 0; i < m_MouseListeners.size(); ++i)
					{
						if (m_MouseListeners[i] != nullptr)
						{
							if (!m_MouseListeners[i]->OnButtonPress(event.mouseButton))
							{
								break;
							}
						}
					}
				} break;
				case sf::Event::MouseButtonReleased:
				{
					for (size_t i = 0; i < m_MouseListeners.size(); ++i)
					{
						if (m_MouseListeners[i] != nullptr)
						{
							m_MouseListeners[i]->OnButtonRelease(event.mouseButton);
						}
					}
				} break;
				case sf::Event::MouseWheelMoved:
				{
					for (size_t i = 0; i < m_MouseListeners.size(); ++i)
					{
						if (m_MouseListeners[i] != nullptr)
						{
							m_MouseListeners[i]->OnScroll(event.mouseWheelScroll);
						}
					}
				} break;
				}
			}
		}

		if (m_Window->hasFocus())
		{
			if (!m_DEBUG_GamePaused || stepOneFrame)
			{
				if (stepOneFrame) accumulator = PhysicsActorManager::TIMESTEP; // Step exactly one frame
				else accumulator += elapsed.asSeconds();

				Tick(accumulator);
			}

			Draw();
		}
	}
}

void ApexMain::Tick(double& accumulator)
{
	while (accumulator >= PhysicsActorManager::TIMESTEP)
	{
		const sf::Time dt = sf::seconds(PhysicsActorManager::TIMESTEP);

		m_StateManager->Tick(dt);
		if (!m_PhysicsPaused) m_PhysicsActorManager->Tick(dt);

		accumulator -= PhysicsActorManager::TIMESTEP;
	}
}

void ApexMain::Draw()
{
	m_Window->clear();
	m_Window->setView(m_Window->getDefaultView());
	m_StateManager->Draw(*m_Window);

	if (m_ShowingPhysicsDebug)
	{
		m_PhysicsActorManager->Draw(*m_Window);
	}

	m_Window->setView(m_Window->getDefaultView());
	m_CursorSprite.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*m_Window)));
	m_Window->draw(m_CursorSprite);

	m_Window->display();
	++m_Frames;
}

sf::Vector2f ApexMain::GetMouseCoordsWorldSpace(sf::View view) const
{
	sf::Vector2i mouse = sf::Mouse::getPosition(*m_Window);
	return m_Window->mapPixelToCoords(mouse, view);
}

sf::Vector2i ApexMain::GetMouseCoordsScreenSpace(sf::View currentView) const
{
	sf::Vector2i mouseCoords = sf::Mouse::getPosition(*m_Window);

	const sf::Vector2u windowSize = m_Window->getSize();
	if (mouseCoords.x < 0 || mouseCoords.x > int(windowSize.x) ||
		mouseCoords.y < 0 || mouseCoords.y > int(windowSize.y))
		return sf::Vector2i(-1, -1);

	if (currentView.getSize() != sf::Vector2f())
	{
		if (m_Window->getSize() != static_cast<sf::Vector2u>(currentView.getSize()))
		{
			// Adjust for when the window has been resized the content stretched or squeezed
			const float xScale = currentView.getSize().x / m_Window->getSize().x;
			const float yScale = currentView.getSize().y / m_Window->getSize().y;
			mouseCoords.x = static_cast<int>(mouseCoords.x * xScale);
			mouseCoords.y = static_cast<int>(mouseCoords.y * yScale);
		}
	}
	mouseCoords += static_cast<sf::Vector2i>((currentView.getCenter() - currentView.getSize() / 2.0f));

	return mouseCoords;
}

void ApexMain::LoadCursorTextures()
{
	m_CursorTextures.resize(int(ApexCursor::_LAST_ELEMENT));
	m_CursorTextures[int(ApexCursor::NORMAL)].loadFromFile("resources/cursor-normal-x2.png");
	m_CursorTextures[int(ApexCursor::POINT)].loadFromFile("resources/cursor-point-x2.png");

	m_CursorSprite.setTexture(m_CursorTextures[int(ApexCursor::NORMAL)]);
}

void ApexMain::SetCursor(ApexCursor cursorType)
{
	m_CursorType = cursorType;
	m_CursorSprite.setTexture(m_CursorTextures[int(m_CursorType)]);
}

void ApexMain::TakeScreenshot()
{
	sf::Image screen = m_Window->capture();
	SYSTEMTIME localTime;
	GetLocalTime(&localTime);

	std::stringstream filenameStream;
	filenameStream << std::to_string(localTime.wYear) << "-";
	filenameStream << std::setw(2) << std::setfill('0') << std::to_string(localTime.wMonth) << "-";
	filenameStream << std::setw(2) << std::setfill('0') << std::to_string(localTime.wDay);
	const std::string dateString = filenameStream.str();

	const std::string path = "screenshots/" + dateString + "_";
	const std::string filetype = ".png";

	// Keep opening files until one doesn't exist
	sf::FileInputStream fileInStream;
	int index = 0;
	while (fileInStream.open(path + std::to_string(index) + filetype))
		++index;

	const std::string filename = path + std::to_string(index) + filetype;
	if (screen.saveToFile(filename))
	{
		ApexOutputDebugString("Saved screenshot as \"" + filename + "\" successfully!\n");
	}
}

StateManager* ApexMain::GetStateManager()
{
	return m_StateManager;
}

sf::Vector2u ApexMain::GetWindowSize() const
{
	return m_Window->getSize();
}

std::string ApexMain::Vector2fToString(sf::Vector2f vec)
{
	std::stringstream stream;
	stream << std::setprecision(2) << std::fixed << vec.x << "," << vec.y;
	return stream.str();
}

void ApexMain::AddKeyListener(ApexKeyListener* keyListener)
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

void ApexMain::RemoveKeyListener(ApexKeyListener * keyListener)
{
	for (size_t i = 0; i < m_KeyListeners.size(); ++i)
	{
		if (m_KeyListeners[i] == keyListener) m_KeyListeners[i] = nullptr;
	}
}

void ApexMain::AddMouseListener(ApexMouseListener* mouseListener)
{
	for (size_t i = 0; i < m_MouseListeners.size(); ++i)
	{
		if (m_MouseListeners[i] == nullptr)
		{
			m_MouseListeners[i] = mouseListener;
			return;
		}
	}
	m_MouseListeners.push_back(mouseListener);
}

void ApexMain::RemoveMouseListener(ApexMouseListener* mouseListener)
{
	for (size_t i = 0; i < m_MouseListeners.size(); ++i)
	{
		if (m_MouseListeners[i] == mouseListener) m_MouseListeners[i] = nullptr;
	}
}

b2World* ApexMain::GetPhysicsWorld() const
{
	return m_PhysicsActorManager->GetWorld();
}

void ApexMain::SetPhysicsPaused(bool physicsPaused)
{
	if (!m_DEBUG_GamePaused)
	{
		m_PhysicsPaused = physicsPaused;
	}
}

bool ApexMain::DEBUGIsGamePaused() const
{
	return m_DEBUG_GamePaused;
}

void ApexMain::ToggleWindowFullscreen()
{
	SetWindowFullscreen(!m_WindowIsFullscreen);
}

void ApexMain::SetWindowFullscreen(bool fullscreen)
{
	if (fullscreen != m_WindowIsFullscreen)
	{
		m_WindowIsFullscreen = fullscreen;
		CreateApexWindow(m_WindowIsFullscreen);
	}
}

sf::Time ApexMain::GetTimeElapsed() const
{
	return m_TotalElapsed;
}

void ApexMain::DEBUGToggleGamePaused()
{
	m_DEBUG_GamePaused = !m_DEBUG_GamePaused;
	if (m_DEBUG_GamePaused)
	{
		m_PhysicsPaused = true;
	}
	else if (!m_DEBUG_GamePaused)
	{
		BaseState* currentState = m_StateManager->CurrentState();
		if (currentState->GetType() == StateType::GAME)
		{
			GameState* gameState = static_cast<GameState*>(currentState);
			if (!gameState->IsLevelPaused())
			{
				m_PhysicsPaused = false;
			}
		}
	}
}

ApexMain* ApexMain::GetSingleton()
{
	if (m_Singleton == nullptr)
	{
		m_Singleton = new ApexMain();
		m_Singleton->Init();
	}
	return m_Singleton;
}