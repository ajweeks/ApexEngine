#pragma once

#include "ApexContactListener.h"
#include "LightManager.h"

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

#include <string>
#include <vector>

class Entity;
class Layer;
class World;
class Mob;
class Item;
class Layer;
struct TileSet;

class Map
{
public:
	Map();
	Map(World* world, int buildingIndex, std::string directory);
	virtual ~Map();

	Map(const Map&) = delete;
	Map& operator=(const Map&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

	void Reset();

	static void LoadShaders();

	int GetTilesWide() const;
	int GetTilesHigh() const;
	int GetTileSize() const;
	sf::Vector2f GetPlayerSpawnPosition() const;

	void ToggleLightingEditor();

	void AddMob(Mob* mob);
	void RemoveMob(Mob* mob);
	void AddMobToBeRemoved(Mob* mob);

	void AddItem(Item* item);
	void RemoveItem(Item* item);
	void AddItemToBeRemoved(Item* item); 

	void CreatePhysicsActors(ApexContactListener* contactListener);
	void DestroyPhysicsActors();

	void InteractWithHighlightedItem();

private:
	Entity* GetNearestInteractableEntityTo(Entity* sourceEntity, float& distance);
	// Extra chars is filled with all characters after one past the begin string 
	//		eg.  StringBeginsWith("door_1", "door", e) -> e = "1"
	bool StringBeginsWith(const std::string& string, const std::string& begin, std::string& extraChars);
	void ReadNPCDataFromFile();

	static sf::Shader s_OutlinedSpriteShader;

	int m_TilesWide;
	int m_TilesHigh;

	int m_BuildingIndex;

	LightManager m_LightManager;

	Layer* m_BackgroundLayer = nullptr;
	Layer* m_MidgroundLayer = nullptr;
	Layer* m_ForegroundLayer = nullptr;

	Entity* m_HighlightedEntity = nullptr;

	std::vector<Mob*> m_Mobs;
	std::vector<Mob*> m_MobsToBeRemoved;
	std::vector<Item*> m_Items;
	std::vector<Item*> m_ItemsToBeRemoved;

	TileSet* m_TileSet = nullptr;
	World* m_World = nullptr;

	sf::Vector2f m_PlayerSpawnPosition;
};
