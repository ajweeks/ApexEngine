#pragma once

#include "ApexContactListener.h"
#include "LightManager.h"

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

#include <string>
#include <vector>

class Entity;
class Building;
class World;
class Mob;
class Item;
class Layer;
struct TileSet;

class Map
{
public:
	Map();
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

	void CreatePhysicsActors(ApexContactListener* contactListener);
	void DestroyPhysicsActors();

	void InteractWithHighlightedItem();

	std::vector<Building*> GetBuildings() const;

	static void SetQuadPosition(sf::Vertex* quad, sf::Vector2f position, int tileSize);
	static void SetQuadPosition(sf::Vertex* quad, float x, float y, int tileSize);
	static void SetQuadTexCoords(sf::Vertex* quad, sf::Vector2f texCoords, int tileSize);
	static void SetQuadTexCoords(sf::Vertex* quad, float x, float y, int tileSize);
	static void SetQuadColor(sf::Vertex* quad, sf::Color color);

	static sf::VertexArray GetSpriteTexture(sf::Vector2u sizeInTiles, TileSet* tileSet, sf::Vector2u tileOffset, sf::Vector2f position);
	static sf::VertexArray GetSpriteTexture(unsigned int tilesWide, unsigned int tilesHigh, TileSet* tileSet, sf::Vector2u tileOffset, sf::Vector2f position);
	static sf::VertexArray GetSpriteTexture(unsigned int tilesWide, unsigned int tilesHigh, TileSet* tileSet, unsigned int tileXOffset, unsigned int tileYOffset, sf::Vector2f position);
	static sf::VertexArray GetSpriteTexture(unsigned int tilesWide, unsigned int tilesHigh, TileSet* tileSet, unsigned int tileXOffset, unsigned int tileYOffset, float x, float y);

private:
	void ReadJSONFile();
	Entity* GetNearestInteractableEntityTo(Entity* sourceEntity, float& distance, sf::Vector2f offset = sf::Vector2f());
	// Extra chars is filled with all characters after one past the begin string 
	//		eg.  StringBeginsWith("door_1", "door", ex) -> ex = "1"
	bool StringBeginsWith(const std::string& string, const std::string& begin, std::string& extraChars = std::string());
	void ReadNPCDataFromFile();

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

	Entity* m_HighlightedEntity = nullptr;

	std::vector<Entity*> m_Entites;
	std::vector<Entity*> m_EntitesToBeRemoved;

	TileSet* m_TileSet = nullptr;

	sf::Vector2f m_PlayerSpawnPosition;
};
