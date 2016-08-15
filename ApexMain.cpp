
#include "ApexMain.h"
#include "LoadingState.h"
#include "MainMenuState.h"
#include "GameState.h"
#include "enumerations.h"
#include "ApexKeyboard.h"
#include "ApexKeyListener.h"
#include "ApexMouse.h"
#include "ApexMouseListener.h"
#include "ApexWindowListener.h"
#include "ApexDebug.h"
#include "ApexAudio.h"
#include "Entity.h"
#include "StateManager.h"
#include "PhysicsActorManager.h"
#include "ApexContactListener.h"
#include "TextureManager.h"
#include "logo.h"

#include <windows.h> // ugh (only required for OutputDebugString I think)
#include <sstream>
#include <iomanip>
#include <filesystem>

#include <Box2D\Dynamics\Contacts\b2Contact.h>

const int ApexMain::INITAL_WINDOW_WIDTH = 2080;
const int ApexMain::INITAL_WINDOW_HEIGHT = 1216;
const std::string ApexMain::WINDOW_TITLE = "Apex Engine";

const bool ApexMain::USE_V_SYNC = true;

ApexMain* ApexMain::m_Singleton = nullptr;
sf::Font ApexMain::FontOpenSans;
sf::Font ApexMain::FontPixelFJ8;

void ApexOutputDebugString(const std::string& string)
{
	OutputDebugStringA(string.c_str());
}

std::vector<std::string> ApexSplit(const std::string& str)
{
	std::vector<std::string> result;
	size_t i = 0;

	while (i != str.size())
	{
		while (i != str.size() && isspace(str[i]))
			++i;

		size_t j = i;
		while (j != str.size() && !isspace(str[j]))
			++j;

		if (i != j)
		{
			result.push_back(str.substr(i, j - i));
			i = j;
		}
	}
	return result;
}

std::vector<std::string> ApexSplit(const std::string& str, const char& delim)
{
	std::vector<std::string> result;
	size_t i = 0;

	while (i != str.size())
	{
		while (i != str.size() && str[i] == delim)
			++i;

		size_t j = i;
		while (j != str.size() && str[j] != delim)
			++j;

		if (i != j)
		{
			result.push_back(str.substr(i, j - i));
			i = j;
		}
	}
	return result;
}

sf::Color SetAlpha(sf::Color color, sf::Uint8 alpha)
{
	return sf::Color(color.r, color.g, color.b, alpha);
}

ApexMain::ApexMain()
{
	if (!sf::Shader::isAvailable())
	{
		throw std::exception("Sorry, your graphics card doesn't support shaders");
	}

	TextureManager::Initialize();
	ApexKeyboard::LoadKeybindingsFromFile();

	m_SlowMoData.Create(0.01f, 1.0f, sf::seconds(0.1f), ApexTransition::EaseType::QUADRATIC_IN_OUT);
	m_SlowMoData.SetFinished();

	srand(static_cast<unsigned>(time(0))); // Seed random number generator

	TransitionData start;
	start.color = sf::Color::White;
	TransitionData end;
	end.color = sf::Color::White;
	m_FadeTransition.Create(start, end, sf::seconds(1));
	m_FadeTransition.SetFinished();
}

ApexMain::~ApexMain()
{
	delete m_StateManager;
	delete m_PhysicsActorManager;
	delete m_Window;

	TextureManager::Destroy();
}

void ApexMain::Init()
{
	m_WindowIsFullscreen = false;
	CreateApexWindow(m_WindowIsFullscreen);
	LoadCursorTextures();

	m_PhysicsActorManager = new PhysicsActorManager(*m_Window);

	m_StateManager = new StateManager(new LoadingState());

	if (!FontOpenSans.loadFromFile("resources/font/OpenSans/OpenSans-Regular.ttf"))
	{
		ApexOutputDebugString("Couldn't load font OpenSans-Regular.ttf!\n");
	}
	if (!FontPixelFJ8.loadFromFile("resources/font/pixelfj8/pixelFJ8.ttf"))
	{
		ApexOutputDebugString("Couldn't load font pixelFJ8.ttf!\n");
	}

	ApexAudio::LoadSounds();
}

void ApexMain::CreateApexWindow(bool fullscreen)
{
	if (m_Window != nullptr) delete m_Window;
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

		m_ElapsedThisFrame += elapsed;
		m_TotalElapsed += elapsed;
		if (m_ElapsedThisFrame.asSeconds() >= 1.0f)
		{
			m_ElapsedThisFrame -= sf::seconds(1.0f);
			m_FPS = m_Frames;
			m_Frames = 0;
			m_UPS = m_Updates;
			m_Updates = 0;
			m_Window->setTitle(WINDOW_TITLE + " - " + std::to_string(m_FPS) + " FPS - " + std::to_string(m_UPS) + " UPS");
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

				for (size_t i = 0; i < m_WindowListeners.size(); i++)
				{
					if (m_WindowListeners[i] != nullptr)
					{
						m_WindowListeners[i]->OnWindowDefocus();
					}
				}
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

					for (size_t i = 0; i < m_WindowListeners.size(); i++)
					{
						if (m_WindowListeners[i] != nullptr)
						{
							m_WindowListeners[i]->OnWindowFocus();
						}
					}
				} break;
				case sf::Event::KeyPressed:
				{
					ApexKeyboard::Key key;
					if (ApexKeyboard::GetKeyFromVKCode(event.key.code, key))
					{
						const bool keyPressed = ApexKeyboard::IsKeyPressed(key);
						for (size_t i = 0; i < m_KeyListeners.size(); ++i)
						{
							if (m_KeyListeners[i] != nullptr)
							{
								if (m_KeyListeners[i]->OnKeyPress(key, keyPressed))
								{
									break;
								}
							}
						}

						if (keyPressed)
						{
							switch (key)
							{
							case ApexKeyboard::SCREENSHOT:
							{
								TakeScreenshot();
							} break;
							case ApexKeyboard::DEBUG_PAUSE_EVERYTHING:
							{
								if (m_StateManager->CurrentState()->GetType() != StateType::LOADING)
								{
									DEBUGToggleGamePaused();
								}
							} break;
							case ApexKeyboard::DEBUG_STEP_ONE_PHYSICS_FRAME:
							{
								if (m_DEBUG_GamePaused) stepOneFrame = true;
							} break;
							case ApexKeyboard::DEBUG_TOGGLE_PHYSICS_OVERLAY:
							{
								m_ShowingPhysicsDebug = !m_ShowingPhysicsDebug;
							} break;
							}
						}
					}
					else
					{
						GameState* gameState = dynamic_cast<GameState*>(m_StateManager->CurrentState());
						if (gameState)
						{
							if (gameState->IsWorldPaused())
							{
								gameState->OnUnmappedKeypress(event.key);
							}
						}
					}
				} break;
				case sf::Event::KeyReleased:
				{
					for (size_t i = 0; i < m_KeyListeners.size(); ++i)
					{
						ApexKeyboard::Key key;
						if (ApexKeyboard::GetKeyFromVKCode(event.key.code, key))
						{
							if (m_KeyListeners[i] != nullptr)
							{
								m_KeyListeners[i]->OnKeyRelease(key);
							}
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
		}
		Draw();
	}
}

void ApexMain::Tick(double& accumulator)
{
	while (accumulator >= PhysicsActorManager::TIMESTEP)
	{
		m_SlowMoData.Tick(sf::seconds(PhysicsActorManager::TIMESTEP));

		float time = PhysicsActorManager::TIMESTEP;

		if (m_SlowMoData.GetPercentComplete() != 1.0f)
		{
			time *= m_SlowMoData.GetCurrentTransitionData();
		}

		const sf::Time dt = sf::seconds(time);

		m_StateManager->Tick(dt);
		m_FadeTransition.Tick(dt);
		if (!m_PhysicsPaused)
		{
			m_PhysicsActorManager->Tick(dt);
			++m_Updates;
		}

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

	const sf::Color color = m_FadeTransition.GetCurrentTransitionData().color;
	if (color != sf::Color::White)
	{
		sf::RectangleShape rect(static_cast<sf::Vector2f>(m_Window->getSize()));
		rect.setFillColor(color);
		sf::RenderStates states;
		states.blendMode = sf::BlendMultiply;
		m_Window->draw(rect, states);
	}

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
	m_CursorTextures.resize(int(ApexCursor::_LAST_ELEMENT), nullptr);
	m_CursorTextures[int(ApexCursor::NORMAL)] = TextureManager::GetTexture(TextureManager::CURSOR_NORMAL);
	m_CursorTextures[int(ApexCursor::POINT)] = TextureManager::GetTexture(TextureManager::CURSOR_POINTER);

	m_CursorSprite.setTexture(*m_CursorTextures[int(ApexCursor::NORMAL)]);
}

void ApexMain::SetCursor(ApexCursor cursorType)
{
	m_CursorType = cursorType;
	m_CursorSprite.setTexture(*m_CursorTextures[int(m_CursorType)]);
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

	if (!std::experimental::filesystem::exists("screenshots")) {
		std::experimental::filesystem::create_directory("screenshots");
	}

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

void ApexMain::AddWindowListener(ApexWindowListener* windowListener)
{
	for (size_t i = 0; i < m_WindowListeners.size(); ++i)
	{
		if (m_WindowListeners[i] == nullptr)
		{
			m_WindowListeners[i] = windowListener;
			return;
		}
	}
	m_WindowListeners.push_back(windowListener);
}

void ApexMain::RemoveWindowListener(ApexWindowListener* windowListener)
{
	for (size_t i = 0; i < m_WindowListeners.size(); ++i)
	{
		if (m_WindowListeners[i] == windowListener) m_WindowListeners[i] = nullptr;
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

		for (size_t i = 0; i < m_WindowListeners.size(); i++)
		{
			if (m_WindowListeners[i] != nullptr)
			{
				m_WindowListeners[i]->OnWindowResize(m_Window->getSize());
			}
		}
	}
}

sf::Time ApexMain::GetTimeElapsed() const
{
	return m_TotalElapsed;
}

sf::RenderWindow* ApexMain::GetWindow()
{
	return m_Window;
}

void ApexMain::SetSlowMoTime(sf::Time duration, ApexTransition::EaseType easeType)
{
	m_SlowMoData.SetDuration(duration);
	m_SlowMoData.SetEaseType(easeType);
	m_SlowMoData.Restart();
}

void ApexMain::SetColorFade(sf::Time length, sf::Color from, sf::Color to, ApexTransition::EaseType easeType)
{
	TransitionData start;
	start.color = from;
	TransitionData end;
	end.color = to;
	m_FadeTransition.Create(start, end, length, easeType);
	m_FadeTransition.Restart();
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
			if (!gameState->IsWorldPaused())
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

void ApexMain::BeginContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	if (fixtureA->GetBody()->GetUserData() != nullptr) // Fixture A is a contact listener
	{
		if (fixtureA->GetUserData() != nullptr && fixtureB->GetUserData() != nullptr)
		{
			ApexContactListener* contactListener = reinterpret_cast<ApexContactListener*>(fixtureA->GetBody()->GetUserData());
			contactListener->BeginContact(
				reinterpret_cast<PhysicsActor*>(fixtureA->GetUserData()),
				reinterpret_cast<PhysicsActor*>(fixtureB->GetUserData()));
		}
	}

	if (fixtureB->GetBody()->GetUserData() != nullptr) // Fixture B is a contact listener
	{
		if (fixtureB->GetUserData() != nullptr && fixtureA->GetUserData() != nullptr)
		{
			ApexContactListener* contactListener = reinterpret_cast<ApexContactListener*>(fixtureB->GetBody()->GetUserData());
			contactListener->BeginContact(
				reinterpret_cast<PhysicsActor*>(fixtureB->GetUserData()),
				reinterpret_cast<PhysicsActor*>(fixtureA->GetUserData()));
		}
	}
}

void ApexMain::EndContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	if (fixtureA->GetBody()->GetUserData() != nullptr) // Fixture A is a contact listener
	{
		if (fixtureA->GetUserData() != nullptr && fixtureB->GetUserData() != nullptr)
		{
			ApexContactListener* contactListener = reinterpret_cast<ApexContactListener*>(fixtureA->GetBody()->GetUserData());
			contactListener->EndContact(
				reinterpret_cast<PhysicsActor *>(fixtureA->GetUserData()),
				reinterpret_cast<PhysicsActor *>(fixtureB->GetUserData()));
		}
	}

	if (fixtureB->GetBody()->GetUserData() != nullptr) // Fixture B is a contact listener
	{
		if (fixtureB->GetUserData() != nullptr && fixtureA->GetUserData() != nullptr)
		{
			ApexContactListener* contactListener = reinterpret_cast<ApexContactListener*>(fixtureB->GetBody()->GetUserData());
			contactListener->EndContact(
				reinterpret_cast<PhysicsActor *>(fixtureB->GetUserData()),
				reinterpret_cast<PhysicsActor *>(fixtureA->GetUserData()));
		}
	}
}

void ApexMain::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	if (fixtureA->GetBody()->GetUserData() != nullptr) // Fixture A is a contact listener
	{
		if (fixtureA->GetUserData() != nullptr && fixtureB->GetUserData() != nullptr)
		{
			bool enableContact = true;
			ApexContactListener* contactListener = reinterpret_cast<ApexContactListener*>(fixtureA->GetBody()->GetUserData());
			contactListener->PreSolve(
				reinterpret_cast<PhysicsActor *>(fixtureA->GetUserData()),
				reinterpret_cast<PhysicsActor *>(fixtureB->GetUserData()),
				enableContact);
			contact->SetEnabled(enableContact);
		}
	}

	if (fixtureB->GetBody()->GetUserData() != nullptr) // Fixture B is a contact listener
	{
		if (fixtureB->GetUserData() != nullptr && fixtureA->GetUserData() != nullptr)
		{
			bool enableContact = true;
			ApexContactListener* contactListener = reinterpret_cast<ApexContactListener*>(fixtureB->GetBody()->GetUserData());
			contactListener->PreSolve(
				reinterpret_cast<PhysicsActor *>(fixtureB->GetUserData()),
				reinterpret_cast<PhysicsActor *>(fixtureA->GetUserData()),
				enableContact);
			contact->SetEnabled(enableContact);
		}
	}
}

void ApexMain::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	// TODO: Implement (?)
}
