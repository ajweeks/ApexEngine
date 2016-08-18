#pragma once

#include "Camera.h"
#include "ApexDebug.h"
#include "PhysicsActorManager.h"
#include "BulletManager.h"
#include "Player.h"
#include "ApexContactListener.h"
#include "ApexKeyListener.h"
#include "ApexParticleManager.h"
#include "IntTransition.h"
#include "ApexKeyboard.h"

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\RenderTexture.hpp>
#include <SFML\Graphics\Sprite.hpp>
#include <SFML\Window\Event.hpp>

#include <JSON\json.hpp>

class Game;
class ApexPauseScreen;
class Minimap;
class HUD;
class Entity;
class Map;

class World : public ApexContactListener, public ApexKeyListener
{
public:
	World(int worldIndex);
	virtual ~World();

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

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

	bool IsShowingSpeechBubble() const;
	void SetCurrentSpeechShowing(const std::string& speech);
	void ClearSpeechShowing();

	void OnUnmappedKeypress(sf::Event::KeyEvent event);

	void EnterMap(int mapIndex);
	void ExitMap();

	int GetWorldIndex() const;

	Map* GetCurrentMap();

	virtual void BeginContact(PhysicsActor* thisActor, PhysicsActor* otherActor);
	virtual void EndContact(PhysicsActor* thisActor, PhysicsActor* otherActor);
	virtual void PreSolve(PhysicsActor* thisActor, PhysicsActor* otherActor, bool& enableContact);

	virtual bool OnKeyPress(ApexKeyboard::Key key, bool keyPressed) override;
	virtual void OnKeyRelease(ApexKeyboard::Key key) override;

private:
	enum class FadingOutTo
	{
		ENTER_BUILDING, EXIT_BUILDING, NONE
	};


	void SetPaused(bool paused, bool pauseSounds);

	void ReadBuildingData();
	void CreateMapPhysicsActors(int mapIndex);
	void DeleteMapPhysicsActors(int mapIndex);
	void DrawSpeechBubble(sf::RenderTarget& target, sf::RenderStates states);

	static const sf::Uint32 MILLISECONDS_PER_SPEECH_BUBBLE_LETTER;

	int m_Width;
	int m_Height;

	int m_WorldIndex;

	Map* m_Map = nullptr;
	std::vector<Map*> m_Maps; // Buildings
	size_t m_CurrentMapShowingIndex = -1;
	size_t m_MapToTravelToIndex = -1;

	Player* m_Player = nullptr;
	Camera* m_Camera = nullptr;
	Minimap* m_Minimap = nullptr;
	HUD* m_HUD = nullptr;

	ApexDebug* m_DebugOverlay = nullptr;
	bool m_ShowingDebugOverlay;
	bool m_Paused;

	ApexParticleManager m_ParticleManager;
	BulletManager* m_BulletManager = nullptr;
	ApexPauseScreen* m_PauseScreen = nullptr;

	ApexSpriteSheet m_SpeechBubbleSpriteSheet;
	std::string m_CurrentSpeech;

	IntTransition m_SpeechLetterTransition;
	FadingOutTo m_FadingOutTo = FadingOutTo::NONE;
};
