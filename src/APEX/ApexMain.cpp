
#include "Apex\ApexMain.h"
#include "Apex\AbstractGame.h"
#include "Apex\State\StateManager.h"
#include "Apex\enumerations.h"
#include "Apex\IO\Keyboard.h"
#include "Apex\IO\KeyListener.h"
#include "Apex\IO\Mouse.h"
#include "Apex\IO\MouseListener.h"
#include "Apex\WindowListener.h"
#include "Apex\IO\Audio.h"
#include "Apex\Physics\PhysicsActorManager.h"
#include "Apex\Physics\ContactListener.h"
#include "Apex\Graphics\TextureManager.h"
#include "Apex\logo.h"
#include "Apex\Physics\PhysicsActor.h"
#include "Apex\Physics\Contact.h"
#include "Apex\Transition\Transition.h"

#include <windows.h> // ugh (only required for OutputDebugString I think)
#include <sstream>
#include <iomanip>
#include <filesystem>

#include <Box2D\Dynamics\Contacts\b2Contact.h>

namespace apex 
{
	const sf::Time ApexMain::FADE_IN_OUT_TIME = sf::seconds(0.35f);

	std::string ApexMain::s_WindowTitle = "Apex Engine";
	bool ApexMain::s_ShowFPSInTitleBar = true;

	ApexMain* ApexMain::s_Singleton = nullptr;
	AbstractGame* ApexMain::s_Game = nullptr;

	void PrintString(const std::string& string, LogType logType)
	{
		// TODO: Store number of errors/warnings logged?
		switch (logType)
		{
		case LogType::LOG_INFO: OutputDebugStringA("[INFO]: "); break;
		case LogType::LOG_WARNING: OutputDebugStringA("[WARNING]: "); break;
		case LogType::LOG_ERROR: OutputDebugStringA("[ERROR]: "); break;
		}
		OutputDebugStringA(string.c_str());
	}

	std::vector<std::string> Split(const std::string& str)
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

	std::vector<std::string> Split(const std::string& str, const char& delim)
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

	sf::Glsl::Vec4 NormalizeColor(const sf::Color& color)
	{
		return sf::Glsl::Vec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
	}

	sf::Vector2f StringToVector2f(const std::string& string)
	{
		const int commaIndex = string.find(',');
		sf::Vector2f result;
		result.x = stof(string.substr(0, commaIndex));
		result.y = stof(string.substr(commaIndex + 1));
		return result;
	}

	std::string Vector2fToString(sf::Vector2f vec)
	{
		std::stringstream stream;
		stream << std::setprecision(2) << std::fixed << vec.x << "," << vec.y;
		return stream.str();
	}


	// Accepts strings in form "rgb(255, 255, 255)" and "rgba(255, 255, 255, 255)"
	sf::Color StringToColor(std::string string)
	{
		sf::Color result;

		if (string.substr(0, 4).compare("rgba") == 0)
		{
			std::string colorValue = string.substr(5, string.length() - 5 - 1);
			colorValue.erase(std::remove_if(colorValue.begin(), colorValue.end(), isspace), colorValue.end()); // remove whitespace
			std::vector<std::string> colors = Split(colorValue, ',');
			result.r = stoi(colors[0]);
			result.g = stoi(colors[1]);
			result.b = stoi(colors[2]);
			result.a = stoi(colors[3]);
		}
		else if (string.substr(0, 3).compare("rgb") == 0)
		{
			std::string colorValue = string.substr(4, string.length() - 4 - 1);
			colorValue.erase(std::remove_if(colorValue.begin(), colorValue.end(), isspace), colorValue.end()); // remove whitespace
			std::vector<std::string> colors = Split(colorValue, ',');
			result.r = stoi(colors[0]);
			result.g = stoi(colors[1]);
			result.b = stoi(colors[2]);
			result.a = 255;
		}
		else
		{
			PrintString("Could not convert string to color: " + string + "\n", LogType::LOG_ERROR);
		}

		return result;
	}

	std::string ColorToString(sf::Color color)
	{
		if (color.a == 255)	return "rgb(" + std::to_string(color.r) + ", " + std::to_string(color.g) + ", " + std::to_string(color.b) + ")";
		return "rgba(" + std::to_string(color.r) + ", " + std::to_string(color.g) + ", " + std::to_string(color.b) + ", " + std::to_string(color.a) + ")";
	}

	ApexMain::ApexMain()
	{
		if (!sf::Shader::isAvailable())
		{
			throw std::exception("Sorry, your graphics card doesn't support shaders");
		}

		m_WindowIsFullscreen = false;
		CreateApexWindow(m_WindowIsFullscreen);

		srand(static_cast<unsigned>(time(0))); // Seed random number generator
	}
	
	ApexMain::~ApexMain()
	{
		delete m_PhysicsActorManager;
		delete m_Window;
	}

	ApexMain* ApexMain::GetSingleton()
	{
		if (s_Singleton == nullptr)
		{
			s_Singleton = new ApexMain();
		}
		return s_Singleton;
	}

	void ApexMain::DestroySingleton()
	{
		delete s_Singleton;
		s_Singleton = nullptr;
	}

	void ApexMain::CreateApexWindow(bool fullscreen)
	{
		if (m_Window != nullptr) delete m_Window;
		if (fullscreen)
		{
			const sf::VideoMode fullscreenMode = sf::VideoMode::getFullscreenModes()[0];
			unsigned int windowWidth = fullscreenMode.width;
			unsigned int windowHeight = fullscreenMode.height;
			m_Window = new sf::RenderWindow(sf::VideoMode(windowWidth, windowHeight, fullscreenMode.bitsPerPixel), "", sf::Style::Fullscreen);
		}
		else
		{
			const sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
			unsigned int windowWidth = (unsigned int)(desktopMode.width * 0.6f);
			unsigned int windowHeight = (unsigned int)(desktopMode.height * 0.6f);
			m_Window = new sf::RenderWindow(sf::VideoMode(windowWidth, windowHeight, desktopMode.bitsPerPixel), "", sf::Style::Close);
		}
		// TODO: Use previously set option here
		//m_Window->setVerticalSyncEnabled(USE_V_SYNC);
		m_Window->setIcon(apex_logo.width, apex_logo.height, apex_logo.pixel_data);
		SetDefaultCursorVisible(!m_CustomCursorVisible);

		UpdateWindowTitle();
	}

	void ApexMain::UpdateWindowTitle()
	{
		std::string windowTitle = s_WindowTitle;
		if (s_ShowFPSInTitleBar) windowTitle += " - FPS: " + std::to_string(m_FPS) + "  UPS: " + std::to_string(m_UPS);
		m_Window->setTitle(windowTitle);
	}

	void ApexMain::Quit()
	{
		m_IsRunning = false;
	}

	void ApexMain::Run(AbstractGame* game)
	{
		s_Game = game;
		m_PhysicsActorManager = new PhysicsActorManager(*m_Window);

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
				UpdateWindowTitle();
			}
		
			if (m_Window->hasFocus())
			{
				Keyboard::Tick();
				Mouse::Tick(elapsed);
			}

			// Process events
			sf::Event event;
			while (m_Window->pollEvent(event))
			{
				if (event.type == sf::Event::LostFocus)
				{
					Audio::SetAllPaused(true);
					Keyboard::Clear();
					Mouse::Clear();

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
						Audio::SetAllPaused(false);

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
						int vkCode;
						if (Keyboard::GetKeyFromVKCode(event.key.code, vkCode))
						{
							const bool keyPressed = Keyboard::IsKeyPressed(vkCode);
							bool blockedInput = false;
							for (size_t i = 0; i < m_KeyListeners.size(); ++i)
							{
								if (m_KeyListeners[i] != nullptr)
								{
									Keyboard::Key key;
									key.vkCode = vkCode;
									if (keyPressed)
									{
										if (m_KeyListeners[i]->OnKeyPress(key, keyPressed))
										{
											blockedInput = true;
											break;
										}

									}
								}
							}
						}
						else
						{
							bool blockedInput = false;
							const bool keyPressed = sf::Keyboard::isKeyPressed(event.key.code);
							for (size_t i = 0; i < m_KeyListeners.size(); ++i)
							{
								if (m_KeyListeners[i] != nullptr)
								{
									if (m_KeyListeners[i]->OnUnmappedKeyPress(event.key.code, keyPressed))
									{
										blockedInput = true;
										break;
									}
								}
							}

						}
					} break;
					case sf::Event::KeyReleased:
					{
						for (size_t i = 0; i < m_KeyListeners.size(); ++i)
						{
							int vkCode;
							if (Keyboard::GetKeyFromVKCode(event.key.code, vkCode))
							{
								if (m_KeyListeners[i] != nullptr)
								{
									Keyboard::Key key;
									key.vkCode = vkCode;
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
								if (m_MouseListeners[i]->OnButtonPress(event.mouseButton))
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
				if (!m_GamePaused || m_StepOneFrame)
				{
					// TODO: FIXME: XXX: Fix timestep
					//if (m_StepOneFrame) 
					//accumulator = PhysicsActorManager::TIMESTEP; // Step exactly one frame
					//else 
					accumulator += elapsed.asSeconds();

					Tick(accumulator);
				}
			}

			Draw();
		}
	}

	void ApexMain::Tick(double& accumulator)
	{
		int tickCount = 0;
		while (accumulator >= PhysicsActorManager::TIMESTEP)
		{
			++tickCount;
			static const sf::Time dt = sf::seconds(PhysicsActorManager::TIMESTEP);

			if (!m_PhysicsPaused)
			{
				m_PhysicsActorManager->Tick(dt);
				++m_Updates;
			}
			
			m_FadeInOutTransitionChain.Tick(dt);
			s_Game->Tick(dt);

			accumulator -= PhysicsActorManager::TIMESTEP;
		}

		Mouse::SetPosLastFrame();
	}

	void ApexMain::Draw()
	{
		const sf::View defaultView = m_Window->getDefaultView();

		m_Window->clear();
		m_Window->setView(defaultView);
		s_Game->Draw(*m_Window);

		if (m_ShowingPhysicsDebug)
		{
			m_PhysicsActorManager->Draw(*m_Window);
		}

		m_Window->setView(defaultView);
		
		if (m_CustomCursorVisible)
		{
			m_CursorSprite.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*m_Window)));
			m_Window->draw(m_CursorSprite);
		}

		// Fade in/out
		std::pair<std::string, apex::Transition*> colorTransition = m_FadeInOutTransitionChain.GetCurrentTransition();
		if (colorTransition.second != nullptr)
		{
			const sf::Color color = static_cast<apex::ColorTransition*>(colorTransition.second)->GetCurrentColor();
			if (color != sf::Color::White)
			{
				sf::RectangleShape rect(static_cast<sf::Vector2f>(m_Window->getSize()));
				rect.setFillColor(color);
				sf::RenderStates states;
				states.blendMode = sf::BlendMultiply;
				m_Window->draw(rect, states);
			}
		}

		m_Window->display();
		++m_Frames;
	}

	void ApexMain::SetShowFPSInTitleBar(bool show)
	{
		s_ShowFPSInTitleBar = show;
		UpdateWindowTitle();
	}

	void ApexMain::SetWindowTitle(const std::string& titleString)
	{
		s_WindowTitle = titleString;
		UpdateWindowTitle();
	}

	void ApexMain::StartFadeInOut(sf::Time length)
	{
		m_FadeInOutTransitionChain.Clear();

		const sf::Color start = sf::Color::White;
		const sf::Color end = sf::Color::Black;
		apex::ColorTransition* fadeInTransition = new apex::ColorTransition(start, end, length);
		m_FadeInOutTransitionChain.AddTransition(fadeInTransition, "in");
		apex::ColorTransition* fadeOutTransition = new apex::ColorTransition(end, start, length);
		m_FadeInOutTransitionChain.AddTransition(fadeOutTransition, "out");
	}

	void ApexMain::StartFadeIn(sf::Time length)
	{
		m_FadeInOutTransitionChain.Clear();

		const sf::Color start = sf::Color::White;
		const sf::Color end = sf::Color::Black;
		apex::ColorTransition* fadeInTransition = new apex::ColorTransition(start, end, length);
		m_FadeInOutTransitionChain.AddTransition(fadeInTransition, "in");
	}

	void ApexMain::StartFadeOut(sf::Time length)
	{
		m_FadeInOutTransitionChain.Clear();

		const sf::Color start = sf::Color::Black;
		const sf::Color end = sf::Color::White;
		apex::ColorTransition* fadeOutTransition = new apex::ColorTransition(start, end, length);
		m_FadeInOutTransitionChain.AddTransition(fadeOutTransition, "out");
	}

	bool ApexMain::IsFadingIn()
	{
		if (m_FadeInOutTransitionChain.IsFinished()) return false;

		std::pair<std::string, apex::Transition*> colorTransition = m_FadeInOutTransitionChain.GetCurrentTransition();
		return colorTransition.first.compare("in") == 0;
	}

	bool ApexMain::IsFadingOut()
	{
		if (m_FadeInOutTransitionChain.IsFinished()) return false;

		std::pair<std::string, apex::Transition*> colorTransition = m_FadeInOutTransitionChain.GetCurrentTransition();
		return colorTransition.first.compare("out") == 0;
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

	sf::Vector2f ApexMain::GetVectorWorldSpace(sf::Vector2i vec, sf::View view)
	{
		return m_Window->mapPixelToCoords(vec, view);
	}

	bool ApexMain::IsMouseInWindow() const
	{
		const sf::Vector2i mousePos = GetMouseCoordsScreenSpace();
		const sf::Vector2i windowSize = static_cast<sf::Vector2i>(m_Window->getSize());
		return (mousePos.x >= 0 && mousePos.y >= 0 && 
			mousePos.x < windowSize.x && mousePos.y < windowSize.y);
	}

	void ApexMain::AddCursorType(Cursor cursorType, sf::Texture* texture)
	{
		const size_t cursorIndex = size_t(cursorType);
		assert(cursorIndex >= 0);
		if (cursorIndex >= m_CursorTextures.size()) m_CursorTextures.resize(cursorIndex + 1);
		m_CursorTextures[cursorIndex] = texture;
		m_CustomCursorVisible = true;
		SetDefaultCursorVisible(!m_CustomCursorVisible);

		// Set the first added cursor as the default
		if (m_CursorTextures.size() == 1) SetCursor(cursorType);
	}

	void ApexMain::SetStepOneFrame(bool stepOneFrame)
	{
		m_StepOneFrame = stepOneFrame;
	}

	bool ApexMain::IsSteppingOneFrame() const
	{
		return m_StepOneFrame;
	}

	void ApexMain::ToggleShowingPhysicsDebug()
	{
		SetShowingPhysicsDebug(!m_ShowingPhysicsDebug);
	}

	void ApexMain::SetShowingPhysicsDebug(bool showing)
	{
		m_ShowingPhysicsDebug = showing;
	}

	bool ApexMain::IsShowingPhysicsDebug() const
	{
		return m_ShowingPhysicsDebug;
	}

	void ApexMain::SetDefaultCursorVisible(bool visible)
	{
		m_Window->setMouseCursorVisible(visible);
	}

	void ApexMain::SetCursor(Cursor cursorType)
	{
		const size_t cursorIndex = size_t(cursorType);
		assert(cursorIndex >= 0 && cursorIndex < m_CursorTextures.size());
		m_CursorSprite.setTexture(*m_CursorTextures[cursorIndex]);
		m_CursorType = cursorType;
	}

	void ApexMain::TakeScreenshot()
	{
		sf::Texture windowTexture;
		const sf::Vector2u windowSize = m_Window->getSize();
		windowTexture.create(windowSize.x, windowSize.y);
		windowTexture.update(*m_Window);
		sf::Image screen = windowTexture.copyToImage();
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
			
			("Saved screenshot as \"" + filename + "\" successfully!\n", LogType::LOG_INFO);
		}
	}

	sf::Vector2u ApexMain::GetWindowSize() const
	{
		return m_Window->getSize();
	}

	void ApexMain::AddKeyListener(KeyListener* keyListener, int priority)
	{
		if (priority == -1)
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
		else
		{
			size_t index = priority;
			if (index >= m_KeyListeners.size()) index = m_KeyListeners.size() - 1;
			std::vector<KeyListener*>::const_iterator iter = m_KeyListeners.begin() + index;
			m_KeyListeners.insert(iter, keyListener);
		}
	}

	void ApexMain::RemoveKeyListener(KeyListener * keyListener)
	{
		for (size_t i = 0; i < m_KeyListeners.size(); ++i)
		{
			if (m_KeyListeners[i] == keyListener) m_KeyListeners[i] = nullptr;
		}
	}

	void ApexMain::AddMouseListener(MouseListener* mouseListener)
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

	void ApexMain::RemoveMouseListener(MouseListener* mouseListener)
	{
		for (size_t i = 0; i < m_MouseListeners.size(); ++i)
		{
			if (m_MouseListeners[i] == mouseListener) m_MouseListeners[i] = nullptr;
		}
	}

	void ApexMain::AddWindowListener(WindowListener* windowListener)
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

	void ApexMain::RemoveWindowListener(WindowListener* windowListener)
	{
		for (size_t i = 0; i < m_WindowListeners.size(); ++i)
		{
			if (m_WindowListeners[i] == windowListener) m_WindowListeners[i] = nullptr;
		}
	}

	b2World& ApexMain::GetPhysicsWorld() const
	{
		return m_PhysicsActorManager->GetWorld();
	}

	void ApexMain::TogglePhysicsPaused()
	{
		SetPhysicsPaused(!m_PhysicsPaused);
	}

	void ApexMain::SetPhysicsPaused(bool physicsPaused)
	{
		if (!m_GamePaused)
		{
			m_PhysicsPaused = physicsPaused;
		}
	}

	bool ApexMain::IsPhysicsPaused() const
	{
		return m_PhysicsPaused;
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

	bool ApexMain::IsWindowFullscreen() const
	{
		return m_WindowIsFullscreen;
	}

	void ApexMain::ToggleVSyncEnabled()
	{
		SetVSyncEnabled(!m_UseVSync);
	}

	void ApexMain::SetVSyncEnabled(bool enabled)
	{
		if (m_UseVSync == enabled) return;

		m_UseVSync = enabled;
		m_Window->setVerticalSyncEnabled(enabled);
	}

	bool ApexMain::IsVSyncEnabled() const
	{
		return m_UseVSync;
	}

	sf::Time ApexMain::GetTimeElapsed() const
	{
		return m_TotalElapsed;
	}

	sf::RenderWindow* ApexMain::GetWindow()
	{
		return m_Window;
	}

	void ApexMain::ToggleGamePaused()
	{
		SetGamePaused(!m_GamePaused);
	}

	void ApexMain::SetGamePaused(bool paused)
	{
		m_GamePaused = !m_GamePaused;
		m_PhysicsPaused = m_GamePaused;

		if (!m_GamePaused) m_StepOneFrame = false;
	}

	bool ApexMain::IsGamePaused() const
	{
		return m_GamePaused;
	}

	void ApexMain::BeginContact(b2Contact* contact)
	{
		Contact apexContact;
		apexContact.fixtureA = contact->GetFixtureA();
		apexContact.fixtureB = contact->GetFixtureB();
		apexContact.actorA =reinterpret_cast<PhysicsActor*>(apexContact.fixtureA->GetUserData());
		apexContact.actorB = reinterpret_cast<PhysicsActor*>(apexContact.fixtureB->GetUserData());

		if (apexContact.fixtureA->GetUserData() != nullptr && apexContact.fixtureB->GetUserData() != nullptr)
		{
			void* fixtureABodyUserData = apexContact.fixtureA->GetBody()->GetUserData();
			if (fixtureABodyUserData != nullptr) // Fixture A is a contact listener
			{
				ContactListener* contactListener = reinterpret_cast<ContactListener*>(fixtureABodyUserData);
				contactListener->BeginContact(&apexContact);
			}

			void* fixtureBBodyUserData = apexContact.fixtureB->GetBody()->GetUserData();
			if (fixtureBBodyUserData != nullptr) // Fixture B is a contact listener
			{
				ContactListener* contactListener = reinterpret_cast<ContactListener*>(fixtureBBodyUserData);
				contactListener->BeginContact(&apexContact);
			}
		}
	}

	void ApexMain::EndContact(b2Contact* contact)
	{
		Contact apexContact;
		apexContact.fixtureA = contact->GetFixtureA();
		apexContact.fixtureB = contact->GetFixtureB();
		apexContact.actorA = reinterpret_cast<PhysicsActor*>(apexContact.fixtureA->GetUserData());
		apexContact.actorB = reinterpret_cast<PhysicsActor*>(apexContact.fixtureB->GetUserData());

		if (apexContact.fixtureA->GetUserData() != nullptr && apexContact.fixtureB->GetUserData() != nullptr)
		{
			void* fixtureABodyUserData = apexContact.fixtureA->GetBody()->GetUserData();
			if (fixtureABodyUserData != nullptr) // Fixture A is a contact listener
			{
				ContactListener* contactListener = reinterpret_cast<ContactListener*>(fixtureABodyUserData);
				contactListener->EndContact(&apexContact);
			}

			void* fixtureBBodyUserData = apexContact.fixtureB->GetBody()->GetUserData();
			if (fixtureBBodyUserData != nullptr) // Fixture B is a contact listener
			{
				ContactListener* contactListener = reinterpret_cast<ContactListener*>(fixtureBBodyUserData);
				contactListener->EndContact(&apexContact);
			}
		}

	}

	void ApexMain::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
	{
		Contact apexContact;
		apexContact.fixtureA = contact->GetFixtureA();
		apexContact.fixtureB = contact->GetFixtureB();
		apexContact.actorA = reinterpret_cast<PhysicsActor*>(apexContact.fixtureA->GetUserData());
		apexContact.actorB = reinterpret_cast<PhysicsActor*>(apexContact.fixtureB->GetUserData());

		if (apexContact.fixtureA->GetUserData() != nullptr && apexContact.fixtureB->GetUserData() != nullptr)
		{
			void* fixtureABodyUserData = apexContact.fixtureA->GetBody()->GetUserData();
			if (fixtureABodyUserData != nullptr) // Fixture A is a contact listener
			{
				bool enableContact = true;
				ContactListener* contactListener = reinterpret_cast<ContactListener*>(fixtureABodyUserData);
				contactListener->PreSolve(&apexContact, enableContact);
				contact->SetEnabled(enableContact);
			}

			void* fixtureBBodyUserData = apexContact.fixtureB->GetBody()->GetUserData();
			if (fixtureBBodyUserData != nullptr) // Fixture B is a contact listener
			{
				bool enableContact = true;
				ContactListener* contactListener = reinterpret_cast<ContactListener*>(fixtureBBodyUserData);
				contactListener->PreSolve(&apexContact, enableContact);
				contact->SetEnabled(enableContact);
			}
		}
	}

	void ApexMain::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
	{
		// TODO: Implement (?)
	}

} // namespace apex