#pragma once

#include "Map.h"
#include "Camera.h"
#include "ApexDebug.h"
#include "PhysicsActorManager.h"
#include "BulletManager.h"
#include "Player.h"
#include "ApexContactListener.h"
#include "ApexWindowListener.h"
#include "ApexParticleManager.h"
#include "LightManager.h"

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\RenderTexture.hpp>
#include <SFML\Graphics\Sprite.hpp>
#include <SFML\Window\Event.hpp>

class Game;
class ApexPauseScreen;
class Mob;
class Minimap;

class Level : public ApexContactListener, public ApexWindowListener
{
public:
	Level(int levelIndex);
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

	void AddParticle(ApexParticle* spriteSheet);

	void RemoveMob(Mob* mob);

	void LoadShaders();
	void LoadLights();
	void ToggleLightingEditor();

	virtual void BeginContact(PhysicsActor* thisActor, PhysicsActor* otherActor);
	virtual void EndContact(PhysicsActor* thisActor, PhysicsActor* otherActor);
	virtual void PreSolve(PhysicsActor* thisActor, PhysicsActor* otherActor, bool& enableContact);

	virtual void OnWindowResize(sf::Vector2u windowSize) override;

private:
	int m_Width;
	int m_Height;

	int m_LevelIndex;

	Map* m_Map = nullptr;
	Player* m_Player = nullptr;
	Camera* m_Camera = nullptr;
	Minimap* m_Minimap = nullptr;

	LightManager m_LightManager;

	BulletManager* m_BulletManager = nullptr;
	std::vector<Mob*> m_Mobs;

	ApexPauseScreen* m_PauseScreen = nullptr;

	ApexDebug* m_DebugOverlay = nullptr;
	bool m_ShowingDebugOverlay = false;
	bool m_Paused = false;

	ApexParticleManager m_ParticleManager;
};
