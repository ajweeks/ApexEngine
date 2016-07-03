#pragma once

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\RenderStates.hpp>
#include <SFML\Window\Event.hpp>

#include "Map.h"
#include "Camera.h"
#include "ApexDebug.h"
#include "PhysicsActorManager.h"
#include "BulletManager.h"
#include "Player.h"

class Game;

class Level
{
public:
	Level();
	virtual ~Level();

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

	unsigned int GetWidth() const;
	unsigned int GetHeight() const;

	void Reset();

	Player* GetPlayer();
	PhysicsActorManager* GetActorManager();
	BulletManager* GetBulletManager();

	void ToggleDebugOverlay();
	bool IsShowingDebugOverlay() const;

	sf::View GetCurrentView() const;

private:
	const int WIDTH;
	const int HEIGHT;

	bool IsPointInPolygon(std::vector<sf::Vector2i> points, sf::Vector2f point);

	Map* m_Map = nullptr;
	Player* m_Player = nullptr;
	Camera* m_Camera = nullptr;

	ApexDebug m_DebugOverlay;
	bool m_ShowingDebugOverlay;

	PhysicsActorManager* m_ActorManager = nullptr;
	BulletManager m_BulletManager;
};
