#pragma once

#include <SFML\Graphics.hpp>
#include "Map.h"
#include "KeyListener.h"
#include "ApexDebug.h"

class Player;
class Game;

class Level : public KeyListener
{
public:
	Level(Game* game);
	virtual ~Level();

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

	unsigned int GetWidth() const;
	unsigned int GetHeight() const;

	virtual void OnKeyPress(sf::Event::KeyEvent keyEvent);
	virtual void OnKeyRelease(sf::Event::KeyEvent keyEvent);

	void Reset();

	Player* GetPlayer();

private:
	const int WIDTH;
	const int HEIGHT;

	void CheckCollisions();
	bool IsPointInPolygon(std::vector<sf::Vector2i> points, sf::Vector2f point);

	Game* m_Game;
	
	sf::Vector2u m_Size;
	sf::View m_View;
	Map m_Map;
	Player* m_Player;

	ApexDebug m_DebugOverlay;
	bool m_IsShowingDebugOverlay;
};
