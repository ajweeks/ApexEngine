#pragma once

#include "Map.h"
#include "Camera.h"
#include "ApexDebug.h"
#include "PhysicsActorManager.h"
#include "BulletManager.h"
#include "Player.h"
#include "ApexContactListener.h"
#include "ApexWindowListener.h"
#include "ApexKeyListener.h"
#include "ApexParticleManager.h"
#include "LightManager.h"
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
class Mob;
class Minimap;
class Item;
class HUD;
class Building;

class World : public ApexContactListener, public ApexWindowListener, public ApexKeyListener
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

	void AddMob(Mob* mob);
	void RemoveMob(Mob* mob);

	void AddItem(Item* item);
	void RemoveItem(Item* item);
	void AddItemToBeRemoved(Item* item); // Call this when Box2D is locked, these items will be deleted next Tick

	void ToggleLightingEditor();

	bool IsShowingSpeechBubble() const;
	void SetCurrentSpeechShowing(const std::string& speech);
	void ClearSpeechShowing();

	void InteractWithHighlightedItem();

	void OnUnmappedKeypress(sf::Event::KeyEvent event);

	void EnterBuilding(int buildingIndex);
	void ExitBuilding();

	virtual void BeginContact(PhysicsActor* thisActor, PhysicsActor* otherActor);
	virtual void EndContact(PhysicsActor* thisActor, PhysicsActor* otherActor);
	virtual void PreSolve(PhysicsActor* thisActor, PhysicsActor* otherActor, bool& enableContact);

	virtual void OnWindowResize(sf::Vector2u windowSize) override;

	virtual bool OnKeyPress(ApexKeyboard::Key key, bool keyPressed) override;
	virtual void OnKeyRelease(ApexKeyboard::Key key) override;

private:
	nlohmann::json GetSpeechDataFromFile();

	Entity* GetNearestInteractableEntityTo(Entity* sourceEntity, float& distance);
	void LoadLights();
	void LoadShaders();
	void SetPaused(bool paused, bool pauseSounds);

	void ReadBuildingData();
	bool CreateBuilding(int buildingIndex);
	void DeleteBuilding(int buildingIndex);

	void CreateMap();

	static const sf::Uint32 MILLISECONDS_PER_SPEECH_BUBBLE_LETTER;

	int m_Width;
	int m_Height;

	int m_WorldIndex;

	Map* m_Map = nullptr;
	Player* m_Player = nullptr;
	Camera* m_Camera = nullptr;
	Minimap* m_Minimap = nullptr;
	HUD* m_HUD = nullptr;

	std::vector<Building*> m_Buildings;
	Building* m_BuildingPlayerIsIn = nullptr;

	LightManager m_LightManager;
	ApexParticleManager m_ParticleManager;
	BulletManager* m_BulletManager = nullptr;
	ApexPauseScreen* m_PauseScreen = nullptr;

	Entity* m_HighlightedEntity = nullptr;
	sf::Shader m_OutlinedSpriteShader;

	std::vector<Mob*> m_Mobs;
	std::vector<Item*> m_Items;
	std::vector<Item*> m_ItemsToBeRemoved;

	ApexDebug* m_DebugOverlay = nullptr;
	bool m_ShowingDebugOverlay;
	bool m_Paused;

	ApexSpriteSheet m_SpeechBubbleSpriteSheet;
	std::string m_CurrentSpeech;

	IntTransition m_SpeechLetterTransition;
};
