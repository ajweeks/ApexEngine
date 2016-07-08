#pragma once

#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\Graphics\Font.hpp>
#include "StateManager.h"
#include "ApexCursor.h"

#define APEX (ApexMain::GetSingleton())

class ApexDebug;
class ApexKeyListener;
class ApexMouseListener;
class PhysicsActorManager;

class b2World;

void OutputDebugString(const std::string &string);

class ApexMain
{
public:
	ApexMain();
	virtual ~ApexMain();

	ApexMain(const ApexMain&) = delete;
	ApexMain& operator=(const ApexMain&) = delete;

	static ApexMain* GetSingleton();

	void Init();

	void Run();
	void Quit();
	sf::Vector2f GetMouseCoordsWorldSpace(sf::View view) const;
	sf::Vector2i GetMouseCoordsScreenSpace(sf::View currentView = sf::View(sf::Vector2f(0, 0), sf::Vector2f(0, 0))) const;

	void SetCursor(sf::ApexCursor::TYPE cursorType);

	static sf::Font FontOpenSans;
	void TakeScreenshot();

	StateManager* GetStateManager();
	sf::Vector2u GetWindowSize() const;

	static std::string Vector2fToString(sf::Vector2f vec);

	void AddKeyListener(ApexKeyListener* keyListener);
	void RemoveKeyListener(ApexKeyListener* keyListener);

	void AddMouseListener(ApexMouseListener* mouseListener);
	void RemoveMouseListener(ApexMouseListener* mouseListener);

	b2World* GetPhysicsWorld() const;

private:
	void Tick(double& accumulator);
	void Draw();

	static const int INITAL_WINDOW_WIDTH;
	static const int INITAL_WINDOW_HEIGHT;
	static const bool USE_V_SYNC;

	static const std::string WINDOW_TITLE;

	static ApexMain* m_Singleton;

	sf::RenderWindow m_Window;
	sf::Time m_TotalElapsed;
	sf::Time m_ElapsedThisFrame;
	int m_Frames;
	int m_FPS;
	bool m_IsRunning;

	bool m_ShowingPhysicsDebug = false;

	StateManager* m_StateManager = nullptr;
	PhysicsActorManager* m_PhysicsActorManager = nullptr;

	std::vector<ApexKeyListener*> m_KeyListeners;
	std::vector<ApexMouseListener*> m_MouseListeners;
};
