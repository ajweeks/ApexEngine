#pragma once

#include "LightManager.h"

#include "Apex\Physics\ContactListener.h"

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

#include <string>
#include <vector>

class Building;
class Entity;
class Layer;
class World;
class Mob;
class Item;
struct TileSet;

class Map
{
public:
	Map(World& world, int buildingIndex, std::string directory);
	virtual ~Map();

	Map(const Map&) = delete;
	Map& operator=(const Map&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states, bool drawLighting);

	void Reset();

	static void LoadShaders();

	int GetTilesWide() const;
	int GetTilesHigh() const;
	int GetTileSize() const;
	TileSet* GetTileSet();
	sf::Vector2f GetPlayerSpawnPosition() const;

	void ToggleLightingEditor();

	void AddEntity(Entity* mob);
	void RemoveEntity(Entity* mob);
	void AddEntityToBeRemoved(Entity* mob);

	void CreatePhysicsActors(apex::ContactListener* contactListener);
	void DestroyPhysicsActors();

	void InteractWithHighlightedItem();
	bool HasHighlightedItem() const;

	std::vector<Building*> GetBuildings() const;

	static void SetQuadPosition(sf::Vertex* quad, sf::Vector2f position, int tileSize);
	static void SetQuadPosition(sf::Vertex* quad, float x, float y, int tileSize);
	static void SetQuadTexCoords(sf::Vertex* quad, sf::Vector2f texCoords, int tileSize);
	static void SetQuadTexCoords(sf::Vertex* quad, float x, float y, int tileSize);
	static void SetQuadColor(sf::Vertex* quad, sf::Color color);

	static sf::VertexArray GetSpriteTexture(sf::Vector2u sizeInTiles, TileSet* tileSet, sf::Vector2u tileOffset, sf::Vector2f position);
	static sf::VertexArray GetSpriteTexture(unsigned int tilesWide, unsigned int tilesHigh, TileSet* tileSet, sf::Vector2u tileOffset, sf::Vector2f position);
	static sf::VertexArray GetSpriteTexture(unsigned int tilesWide, unsigned int tilesHigh, TileSet* tileSet, unsigned int tileXOffset, unsigned int tileYOffset, sf::Vector2f position);
	static sf::VertexArray GetSpriteTexture(unsigned int tilesWide, unsigned int tilesHigh, TileSet* tileSet, unsigned int tileXOffset, unsigned int tileYOffset, float left, float top);

private:
	void GetNPCDataFromFile();
	void GetTileDataFromFile();
	void GetEntityTypesFromFile(); // TODO: Make this static? (only call it once per game run)
	void GetEntityDataFromFile();
	Entity* GetNearestInteractableEntityTo(Entity* sourceEntity, float& distance, sf::Vector2f offset = sf::Vector2f());
	// Extra chars is filled with all characters after one past the begin string 
	//		eg.  StringBeginsWith("door_1", "door", ex) -> ex = "1"
	bool StringBeginsWith(const std::string& string, const std::string& begin, std::string& extraChars = std::string());
	void EntityEditor();

	static sf::Shader s_OutlinedSpriteShader;

	static const float DIST_TO_INTERACTIBLE_ENTITY;

	int m_TilesWide;
	int m_TilesHigh;

	int m_BuildingIndex; // What building this map represents (-1 if outside)

	World& m_World;

	std::string m_Directory;

	LightManager m_LightManager;

	Layer* m_BackgroundLayer = nullptr;
	Layer* m_ForegroundLayer = nullptr;

	std::vector<Entity*> m_Entites;
	std::vector<Entity*> m_EntitesToBeRemoved;
	Entity* m_HighlightedEntity = nullptr;

	struct EntityType
	{
		//Entity* (*Create)(World& world, Map& map, sf::Vector2f position, ActorID id, void* userPointer, b2BodyType bodyType);
		std::string name;
		sf::Vector2u size;
		sf::Vector2u tileXY;
		int id = -1;
		// NOTE: Not all entities have the following properties (and none have all of them)
		sf::Vector2f doorPos;
		// icon?
	};

	std::multimap<std::string, EntityType> m_EntityTypes;

	TileSet* m_TileSet = nullptr;

	sf::Vector2f m_PlayerSpawnPosition;
};
