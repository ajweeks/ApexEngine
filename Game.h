#pragma once

#include <SFML\Graphics.hpp>
#include "StateManager.h"
#include "StandardCursor.hpp"

class ApexDebug;

class Game
{
public:
	Game();
	virtual ~Game();

	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;

	void Run();
	sf::Vector2f MapPixelToCoords(sf::Vector2i) const;
	sf::Vector2f GetMouseCoordsWorldSpace() const;
	sf::Vector2i GetMouseCoordsScreenSpace() const;

	void SetCursor(sf::StandardCursor::TYPE cursorType);

	static sf::Font font12;
	void TakeScreenshot();

private:
	static const int INITAL_WINDOW_WIDTH;
	static const int INITAL_WINDOW_HEIGHT;
	static const bool USE_V_SYNC;

	static const sf::String WINDOW_TITLE;

	void IntializeDebug();
	void Tick(sf::Time elapsed);
	void Draw();

	StateManager m_StateManager;
	sf::RenderWindow m_Window;
	sf::Time m_TotalElapsed;
	sf::Time m_ElapsedThisFrame;
	int m_Frames;
	int m_FPS;

	sf::Font m_Font;

	ApexDebug* m_Debug;
	bool m_ShowDebug = true;
};
