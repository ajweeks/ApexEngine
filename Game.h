#pragma once

#include <SFML\Graphics.hpp>
#include "StateManager.h"
#include "ApexCursor.h"

class ApexDebug;
class KeyListener;

class Game
{
public:
	Game();
	virtual ~Game();

	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;

	void Run();
	sf::Vector2f GetMouseCoordsWorldSpace(sf::View view) const;
	sf::Vector2i GetMouseCoordsScreenSpace(sf::View currentView) const;

	void SetCursor(sf::ApexCursor::TYPE cursorType);

	static sf::Font font12;
	void TakeScreenshot();

	StateManager* GetStateManager();
	sf::Vector2u GetWindowSize() const;

	static std::string Vector2fToString(sf::Vector2f vec);

	void AddKeyListener(KeyListener* keyListener);
	void RemoveKeyListener(KeyListener* keyListener);

private:
	static const int INITAL_WINDOW_WIDTH;
	static const int INITAL_WINDOW_HEIGHT;
	static const bool USE_V_SYNC;

	static const std::string WINDOW_TITLE;

	void Tick(sf::Time elapsed);
	void Draw();

	sf::RenderWindow m_Window;
	sf::Time m_TotalElapsed;
	sf::Time m_ElapsedThisFrame;
	int m_Frames;
	int m_FPS;

	StateManager* m_StateManager = nullptr;
	sf::Font m_Font;

	std::vector<KeyListener*> m_KeyListeners;
};
