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
#include "ApexContactListener.h"

class Game;
class ApexPauseScreen;
class Mob;

class Level : public ApexContactListener
{
public:
	Level();
	virtual ~Level();

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);
	void DrawMap(sf::RenderTarget& target, sf::RenderStates states);

	unsigned int GetWidth() const;
	unsigned int GetHeight() const;

	void Reset();

	Player* GetPlayer();
	BulletManager* GetBulletManager();

	void ToggleDebugOverlay();
	bool IsShowingDebugOverlay() const;

	sf::View GetCurrentView() const;

	void JoltCamera(float xAmount, float yAmount);
	void SetScreenShake(float xScale, float yScale);

	void TogglePaused(bool pauseSounds);
	bool IsPaused() const;

	void RemoveMob(Mob* mob);

	virtual void BeginContact(PhysicsActor* thisActor, PhysicsActor* otherActor);
	virtual void EndContact(PhysicsActor* thisActor, PhysicsActor* otherActor);
	virtual void PreSolve(PhysicsActor* thisActor, PhysicsActor* otherActor, bool& enableContact);

private:
	int m_Width;
	int m_Height;

	Map* m_Map = nullptr;
	Player* m_Player = nullptr;
	Camera* m_Camera = nullptr;

	BulletManager* m_BulletManager = nullptr;
	std::vector<Mob*> m_Mobs;

	ApexPauseScreen* m_PauseScreen = nullptr;

	ApexDebug* m_DebugOverlay = nullptr;
	bool m_ShowingDebugOverlay = false;
	bool m_Paused = false;
};
