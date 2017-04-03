
#include "Game\World\Map.h"
#include "Game\World\Layer.h"
#include "Game\World\Tile.h"
#include "Game\World\AICharacter.h"
#include "Game\World\World.h"
#include "Game\World\Item.h"
#include "Game\World\Bed.h"
#include "Game\World\Coin.h"
#include "Game\World\Building.h"
#include "Game\World\ExitSensor.h"
#include "Game\World\SolidBlock.h"
#include "Game\World\Layer.h"
#include "Game\World\LightManager.h"
#include "Game\World\TileSet.h"
#include "Game\World\Interactable.h"
#include "Game\Debug\ImGui.h"

#include "Apex\ApexMain.h"
#include "Apex\Graphics\TextureManager.h"
#include "Apex\ApexMath.h"
#include "Apex\Physics\PhysicsActor.h"

#include <JSON\json.hpp>

#include <fstream>
#include <map>

sf::Shader Map::s_OutlinedSpriteShader;

const float Map::DIST_TO_INTERACTIBLE_ENTITY = 35.0f;

using json = nlohmann::json;

Map::Map(World& world, int buildingIndex, std::string directory) :
	m_LightManager(world, directory),
	m_World(world),
	m_BuildingIndex(buildingIndex),
	m_Directory(directory)
{
}

Map::~Map()
{
	delete m_BackgroundLayer;
	delete m_ForegroundLayer;

	delete m_TileSet;

	for (size_t i = 0; i < m_EntitesToBeRemoved.size(); ++i)
	{
		delete m_EntitesToBeRemoved[i];
	}
	m_EntitesToBeRemoved.clear();

	for (size_t i = 0; i < m_Entites.size(); i++)
	{
		if (m_Entites[i] != nullptr)
		{
			delete m_Entites[i];
		}
	}
	m_Entites.clear();

	m_HighlightedEntity = nullptr;
}

void Map::Reset()
{
	DestroyPhysicsActors();

	for (size_t i = 0; i < m_EntitesToBeRemoved.size(); i++)
	{
		delete m_EntitesToBeRemoved[i];
	}
	m_EntitesToBeRemoved.clear();

	for (size_t i = 0; i < m_Entites.size(); i++)
	{
		delete m_Entites[i];
	}
	m_Entites.clear();

	delete m_BackgroundLayer;
	m_BackgroundLayer = nullptr;

	delete m_ForegroundLayer;
	m_ForegroundLayer = nullptr;

	delete m_TileSet;
	m_TileSet = nullptr;

	m_HighlightedEntity = nullptr;

	GetNPCDataFromFile();
	GetTileDataFromFile();
	GetEntityTypesFromFile();
	GetEntityDataFromFile();

	m_LightManager.LoadLightData();
}

void Map::GetTileDataFromFile()
{
	const std::string fileName = "tiles.json";
	const std::string filePath = m_Directory + fileName;

	std::ifstream fileInStream;
	fileInStream.open(filePath);

	if (!fileInStream)
	{
		std::cout << "Couldn't find file \"" << filePath << "\"!" << std::endl;
		return;
	}

	std::string line;
	std::stringstream wholeFileStringStream;
	while (!fileInStream.eof())
	{
		std::getline(fileInStream, line);
		wholeFileStringStream << line;
	}
	fileInStream.close();

	std::string wholeFileString = wholeFileStringStream.str();
	wholeFileString.erase(std::remove_if(wholeFileString.begin(), wholeFileString.end(), isspace), wholeFileString.end());

	json tileMap = json::parse(wholeFileString);
	if (tileMap.empty())
	{
		std::cout << "Map file \"" << filePath << "\" was inavlid!" << std::endl;
		return;
	}

	std::string orientation = tileMap["orientation"];
	if (orientation.compare("orthogonal") != 0)
	{
		std::cout << "Non-orthogonal map passed in! The type specified was \"" << orientation << "\"!" << std::endl;
		return;
	}

	m_TilesWide = tileMap["width"];
	m_TilesHigh = tileMap["height"];

	std::vector<json> tileSets = tileMap["tilesets"];
	if (tileSets.size() != 1)
	{
		std::cout << "More than one tile set in map! Please only use one!" << std::endl;
		return;
	}
	const int tileSetTileSize = tileSets[0]["tilewidth"];
	const int tileSetSpacing = tileSets[0]["spacing"];
	const int tileSetMargin = tileSets[0]["margin"];
	const int tileCount = tileSets[0]["tilecount"];
	json tileProperties = tileSets[0]["tileproperties"];

	std::vector<Tile*> tileAtlas;
	tileAtlas.reserve(tileCount);
	for (int i = 0; i < tileCount; ++i)
	{
		const int ID = i - 1;
		bool solid = false;
		bool sensor = false;
		std::string tileStringID;
		const std::string tileIndexString = std::to_string(ID);
		if (tileProperties.find(tileIndexString) != tileProperties.end())
		{
			const json currentTile = tileProperties[tileIndexString];
			if (currentTile.find("solid") != currentTile.end())
			{
				solid = currentTile["solid"].get<bool>();
			}
			if (currentTile.find("sensor") != currentTile.end())
			{
				sensor = currentTile["sensor"].get<bool>();
			}
			if (currentTile.find("id") != currentTile.end())
			{
				tileStringID = currentTile["id"].get<std::string>();
			}
		}
		Tile* tile = new Tile(ID, solid, sensor);
		if (!tileStringID.empty())
		{
			tile->SetStringID(tileStringID);
		}
		tileAtlas.push_back(tile);
	}

	m_TileSet = new TileSet(apex::TextureManager::GetTexture(Texture::GENERAL_TILES), tileSetTileSize, tileSetMargin, tileSetSpacing);

	json layers = tileMap["layers"];
	for (json::iterator i = layers.begin(); i != layers.end(); ++i)
	{
		json currentLayer = *i;
		const std::string layerTypeString = currentLayer["type"].get<std::string>();
		const Layer::Type layerType = Layer::ParseLayerTypeString(layerTypeString);
		const std::string layerName = currentLayer["name"];
		const bool layerVisible = currentLayer["visible"];
		const float layerOpacity = currentLayer["opacity"];
		const int layerWidth = currentLayer["width"];
		const int layerHeight = currentLayer["height"];

		switch (layerType)
		{
		case Layer::Type::TILE:
		{
			const std::vector<int> tileIDs = currentLayer["data"];

			if (layerName.compare("midground") == 0)
			{
				for (size_t i = 0; i < tileIDs.size(); ++i)
				{
					const int tileSize = 16;
					const int x = i % layerWidth;
					const int y = i / layerWidth;
					const sf::Vector2f topLeft = sf::Vector2f(float(x * tileSize), float(y * tileSize));
					const std::string stringID = tileAtlas[tileIDs[i]]->GetStringID();
					std::string ex;
					if (StringBeginsWith(stringID, "solid_block", ex))
					{
						SolidBlock* solidBlock = new SolidBlock(m_World, *this, topLeft);
						m_Entites.push_back(solidBlock);
					}
				}
			}
			else
			{
				std::vector<Tile*> tiles;
				tiles.reserve(tileIDs.size());
				for (size_t i = 0; i < tileIDs.size(); ++i)
				{
					const int id = tileIDs[i];
					Tile* tile = new Tile(id, tileAtlas[id]->IsSolid(), tileAtlas[id]->IsSensor());
					tiles.push_back(tile);
				}

				Layer* newLayer = new Layer(m_World, tiles, m_TileSet, layerName, layerVisible,
					layerOpacity, layerType, layerWidth, layerHeight);

				if (layerName.compare("background") == 0)
				{
					assert(m_BackgroundLayer == nullptr); // ERROR: Multiple background layers found in filePath!
					m_BackgroundLayer = newLayer;
				}
				else if (layerName.compare("foreground") == 0)
				{
					assert(m_ForegroundLayer == nullptr); // ERROR: Multiple foreground layers found in filePath!
					m_ForegroundLayer = newLayer;
				}
				else apex::PrintString("ERROR: Unhandled layer name! " + layerName + "\n", apex::LogType::LOG_ERROR);
			}
		} break;
		case Layer::Type::OBJECT:
		{
			// Skip image layers for now
		} break;
		case Layer::Type::IMAGE:
		{
			// Skip object layers for now
		}
		case Layer::Type::NONE:
		default:
		{
		} break;
		}
	}

	if (tileMap["properties"].find("player_spawn") != tileMap["properties"].end())
	{
		m_PlayerSpawnPosition = apex::StringToVector2f(tileMap["properties"]["player_spawn"].get<std::string>());
		m_PlayerSpawnPosition.x *= m_TileSet->m_TileSize;
		m_PlayerSpawnPosition.y *= m_TileSet->m_TileSize;
	}

	// For now, all layers must be included in map files
	assert(m_BackgroundLayer != nullptr && m_ForegroundLayer != nullptr);

	for (size_t i = 0; i < tileAtlas.size(); ++i)
	{
		delete tileAtlas[i];
	}
	tileAtlas.clear();
}

void Map::GetEntityTypesFromFile()
{
	std::ifstream inputStream;

	inputStream.open("resources/entity_types.json");
	if (inputStream.is_open())
	{
		std::string line;
		std::stringstream fileContentsStream;
		while (std::getline(inputStream, line))
		{
			fileContentsStream << line;
		}

		json entityTypesFileJson = json::parse(fileContentsStream.str());
		std::vector<json> entityTypes = entityTypesFileJson["entity-types"].get<std::vector<json>>();
		for (size_t i = 0; i < entityTypes.size(); i++)
		{
			EntityType type;
			type.name = entityTypes[i]["name"].get<std::string>();
			type.size = static_cast<sf::Vector2u>(apex::StringToVector2f(entityTypes[i]["size"].get<std::string>()));
			// TODO: Make overloaded toVector2u function
			type.tileXY = static_cast<sf::Vector2u>(apex::StringToVector2f(entityTypes[i]["tileXY"].get<std::string>()));
			if (entityTypes[i].find("doorPos") != entityTypes[i].end())
			{
				type.doorPos = apex::StringToVector2f(entityTypes[i]["doorPos"].get<std::string>());
			}
			if (entityTypes[i].find("id") != entityTypes[i].end())
			{
				type.id = entityTypes[i]["id"].get<int>();
			}
			m_EntityTypes.emplace(type.name, type);
		}

		inputStream.close();
	}
	else
	{
		apex::PrintString("Unable to load entity types from file!", apex::LogType::LOG_ERROR);
		return;
	}
}

void Map::GetEntityDataFromFile()
{
	std::ifstream inputStream;

	inputStream.open(m_Directory + "entities.json");
	if (inputStream.is_open())
	{
		std::string line;
		std::stringstream fileContentsStream;
		while (std::getline(inputStream, line))
		{
			fileContentsStream << line;
		}

		json entitiesFileJson = json::parse(fileContentsStream.str());
		std::vector<json> entities = entitiesFileJson["entities"].get<std::vector<json>>();
		for (size_t i = 0; i < entities.size(); i++)
		{
			json currentEntityJson = entities[i];

			const std::string type = currentEntityJson["type"].get<std::string>();
			const int id = currentEntityJson["id"].get<int>();
			const sf::Vector2f pos = apex::StringToVector2f(currentEntityJson["position"].get<std::string>()) * float(m_TileSet->m_TileSize);
			EntityType entityType = m_EntityTypes.find(type)->second;
			if (entityType.id != -1) // there are more than one types with this name
			{
				auto range = m_EntityTypes.equal_range(type);
				for (auto it = range.first; it != range.second; ++it)
				{
					if (it->second.id == id)
					{
						entityType = it->second;
						break;
					}
				}
			}
			const sf::Vector2f doorPos = entityType.doorPos;
			const sf::Vector2u tileXY = entityType.tileXY;
			const sf::Vector2u size = entityType.size;

			// TOOD: Determine texture to use dynamically (without using spritesheet file path)
			sf::Texture* texture = apex::TextureManager::GetTexture(Texture::BUILDINGS);

			Entity* entity = nullptr;
			if (type.compare("building") == 0)
			{
				entity = new Building(m_World, *this, pos, texture, size, tileXY, doorPos);
			} 
			else if (type.compare("bed") == 0)
			{
				entity = new Bed(m_World, *this, pos);
			}
			else if (type.compare("coin") == 0)
			{
				entity = new Coin(m_World, *this, pos);
			}
			else
			{
				apex::PrintString("Unknown entity type: " + type, apex::LogType::LOG_ERROR);
			}
			m_Entites.push_back(entity);
		}

		inputStream.close();
	}
	else
	{
		apex::PrintString("Unable to load entities from file! Directory given: " + m_Directory, apex::LogType::LOG_ERROR);
		return;
	}
}

void Map::LoadShaders()
{
	if (!s_OutlinedSpriteShader.loadFromFile("resources/shaders/outline_sprite.frag", sf::Shader::Fragment))
	{
		apex::PrintString("\n\n\n\tCould not either load or compile outline_sprite.frag\n\n\n\n", apex::LogType::LOG_ERROR);
	}
	s_OutlinedSpriteShader.setUniform("u_color", sf::Glsl::Vec4(1.0f, 1.0f, 1.0f, 0.95f));
}

void Map::Tick(sf::Time elapsed)
{
	for (size_t i = 0; i < m_EntitesToBeRemoved.size(); ++i)
	{
		delete m_EntitesToBeRemoved[i];
	}
	m_EntitesToBeRemoved.clear();
	
	m_BackgroundLayer->Tick(elapsed);
	m_ForegroundLayer->Tick(elapsed);

	for (size_t i = 0; i < m_Entites.size(); i++)
	{
		if (m_Entites[i] != nullptr)
		{
			m_Entites[i]->Tick(elapsed);
		}
	}

	m_LightManager.Tick(elapsed);

	float distanceToNearestEntity;
	const sf::Vector2f offset(0, -20.0f);
	Entity* nearestEntity = GetNearestInteractableEntityTo(m_World.GetPlayer(), distanceToNearestEntity, offset);
	if (distanceToNearestEntity != -1.0f && distanceToNearestEntity <= DIST_TO_INTERACTIBLE_ENTITY)
	{
		m_HighlightedEntity = nearestEntity;
	}
	else
	{
		m_HighlightedEntity = nullptr;
	}

	// Somehow the player moved away while talking with someone, clear the speech bubble
	if (m_HighlightedEntity == nullptr && m_World.IsShowingSpeechBubble())
	{
		m_World.ClearSpeechShowing();
	}

#if 0
	// Rainbow outline
	const float time = APEX->GetTimeElapsed().asSeconds();
	const sf::Uint8 outlineR = sf::Uint8((sin(time * 15.0f + 1.2f) + 1.0f) * 127);
	const sf::Uint8 outlineG = sf::Uint8((cos(time * 20.5f) + 1.0f) * 127);
	const sf::Uint8 outlineB = sf::Uint8((sin(time * 12.0f + 0.5f) + 1.0f) * 127);
	s_OutlinedSpriteShader.setParameter("u_color", sf::Color(outlineR, outlineG, outlineB));
#endif

	if (Game::IsShowingDebugOverlay())
	{
		EntityEditor();
	}
}

void Map::EntityEditor()
{
	// ImGui::Begin() ?
	if (ImGui::CollapsibleElement("Entity editor"))
	{
		for (auto iter = m_EntityTypes.begin(); iter != m_EntityTypes.end(); ++iter)
		{
			ImGui::Text("Type: " + iter->second.name);
			ImGui::Text("Size: " + std::to_string(iter->second.size.x) + ", " + std::to_string(iter->second.size.y));
			ImGui::NewLine();
			// TODO: Add selectability to items
		}
	}
	ImGui::CollapsibleElementPop();
	// ImGui::End() ?
}

/*
	1. Draw background tiles
	2. Sort all items/mobs/player by their y pos
	3. Draw items/mobs/player from back to front
	4. Draw foreground tiles
	5. Draw lights
*/
void Map::Draw(sf::RenderTarget& target, sf::RenderStates states, bool drawLighting)
{
	m_BackgroundLayer->Draw(target, states);

	std::multimap<float, Entity*> sortedMidgroundEntities; // sorted by y value
	typedef std::multimap<float, Entity*>::iterator iter;
	for (size_t i = 0; i < m_Entites.size(); i++)
	{
		if (m_Entites[i] != nullptr)
		{
			sortedMidgroundEntities.emplace(m_Entites[i]->GetBottomMiddlePoint().y, m_Entites[i]);
		}
	}
	sortedMidgroundEntities.emplace(m_World.GetPlayer()->GetBottomMiddlePoint().y, m_World.GetPlayer());

	for (iter& it = sortedMidgroundEntities.begin(); it != sortedMidgroundEntities.end(); ++it)
	{
		if (m_HighlightedEntity == it->second)
		{
			states.shader = &s_OutlinedSpriteShader;
			m_HighlightedEntity->Draw(target, states);
			states.shader = states.Default.shader;
		}
		else
		{
			it->second->Draw(target, states);
		}
	}

	m_ForegroundLayer->Draw(target, states);

	if (drawLighting)
	{
		m_LightManager.Draw(target, states);
	}
}

Entity* Map::GetNearestInteractableEntityTo(Entity* sourceEntity, float& distance, sf::Vector2f offset)
{
	Entity* nearestSoFar = nullptr;
	distance = -1.0f;

	const sf::Vector2f sourcePos = sourceEntity->GetPhysicsActor()->GetPosition() + offset;
	for (size_t i = 0; i < m_Entites.size(); ++i)
	{
		Interactable* interactable = dynamic_cast<Interactable*>(m_Entites[i]);
		if (interactable)
		{
			const sf::Vector2f otherPos = m_Entites[i]->GetPhysicsActor()->GetPosition();
			const float currentDistance = apex::ApexMath::Distance(sourcePos, otherPos);

			if (distance == -1.0f || currentDistance < distance)
			{
				distance = currentDistance;
				nearestSoFar = m_Entites[i];
			}
		}
	}

	return nearestSoFar;
}

void Map::InteractWithHighlightedItem()
{
	if (m_HighlightedEntity != nullptr)
	{
		dynamic_cast<Interactable*>(m_HighlightedEntity)->Interact();
	}
}

bool Map::HasHighlightedItem() const
{
	return m_HighlightedEntity != nullptr;
}

std::vector<Building*> Map::GetBuildings() const
{
	std::vector<Building*> buildings;

	for (Entity* entity : m_Entites)
	{
		Building* building = dynamic_cast<Building*>(entity);
		if (building != nullptr)
		{
			buildings.push_back(building);
		}
	}

	return buildings;
}

sf::VertexArray Map::GetSpriteTexture(sf::Vector2u sizeInTiles, TileSet* tileSet, sf::Vector2u tileOffset, sf::Vector2f position)
{
	return GetSpriteTexture(sizeInTiles.x, sizeInTiles.y, tileSet, tileOffset.x, tileOffset.y, position.x, position.y);
}

sf::VertexArray Map::GetSpriteTexture(unsigned int tilesWide, unsigned int tilesHigh, TileSet* tileSet, sf::Vector2u tileOffset, sf::Vector2f position)
{
	return GetSpriteTexture(tilesWide, tilesHigh, tileSet, tileOffset.x, tileOffset.y, position.x, position.y);
}

sf::VertexArray Map::GetSpriteTexture(unsigned int tilesWide, unsigned int tilesHigh, TileSet * tileSet, unsigned int tileXOffset, unsigned int tileYOffset, sf::Vector2f position)
{
	return GetSpriteTexture(tilesWide, tilesHigh, tileSet, tileXOffset, tileYOffset, position.x, position.y);
}

sf::VertexArray Map::GetSpriteTexture(unsigned int tilesWide, unsigned int tilesHigh, TileSet * tileSet, 
	unsigned int tileXOffset, unsigned int tileYOffset, float left, float top)
{
	sf::VertexArray verticies;
	verticies.setPrimitiveType(sf::Quads);
	const int numTiles = tilesWide * tilesHigh;
	verticies.resize(numTiles * 4);

	const int tileSize = tileSet->m_TileSize;
	const sf::Vector2f posOffset = sf::Vector2f(left, top) / float(tileSize);
	for (unsigned int y = 0; y < tilesHigh; ++y)
	{
		for (unsigned int x = 0; x < tilesWide; ++x)
		{
			const int tileIndex = (x + y * tilesWide);
			sf::Vertex* currentQuad = &verticies[tileIndex * 4];
			SetQuadPosition(currentQuad, posOffset.x + x, posOffset.y + y, tileSize);

			const int tileSrcX = tileXOffset + x;
			const int tileSrcY = tileYOffset + y;
			SetQuadTexCoords(currentQuad, float(tileSrcX), float(tileSrcY), tileSize);
		}
	}
	return verticies;
}

void Map::SetQuadPosition(sf::Vertex* quad, sf::Vector2f position, int tileSize)
{
	SetQuadPosition(quad, position.x, position.y, tileSize);
}

void Map::SetQuadPosition(sf::Vertex* quad, float x, float y, int tileSize)
{
	quad[0].position = sf::Vector2f(x * tileSize, y * tileSize);
	quad[1].position = sf::Vector2f((x + 1) * tileSize, y * tileSize);
	quad[2].position = sf::Vector2f((x + 1) * tileSize, (y + 1) * tileSize);
	quad[3].position = sf::Vector2f(x * tileSize, (y + 1) * tileSize);
}

void Map::SetQuadTexCoords(sf::Vertex* quad, sf::Vector2f texCoords, int tileSize)
{
	SetQuadTexCoords(quad, texCoords.x, texCoords.y, tileSize);
}

void Map::SetQuadTexCoords(sf::Vertex* quad, float x, float y, int tileSize)
{
	quad[0].texCoords = sf::Vector2f(x * tileSize, y * tileSize);
	quad[1].texCoords = sf::Vector2f((x + 1) * tileSize, y * tileSize);
	quad[2].texCoords = sf::Vector2f((x + 1) * tileSize, (y + 1) * tileSize);
	quad[3].texCoords = sf::Vector2f(x * tileSize, (y + 1) * tileSize);
}

void Map::SetQuadColor(sf::Vertex* quad, sf::Color color)
{
	quad[0].color = color;
	quad[1].color = color;
	quad[2].color = color;
	quad[3].color = color;
}

int Map::GetTileSize() const
{
	return m_TileSet->m_TileSize;
}

TileSet* Map::GetTileSet()
{
	return m_TileSet;
}

void Map::CreatePhysicsActors(apex::ContactListener* contactListener)
{
	m_BackgroundLayer->CreatePhysicsActors(contactListener);
	m_ForegroundLayer->CreatePhysicsActors(contactListener);

	for (size_t i = 0; i < m_Entites.size(); i++)
	{
		if (m_Entites[i] != nullptr)
		{
			m_Entites[i]->CreatePhysicsActor();
		}
	}
}

void Map::DestroyPhysicsActors()
{
	if (m_BackgroundLayer != nullptr)
	{
		m_BackgroundLayer->DestroyPhysicsActors();
	}
	if (m_ForegroundLayer != nullptr)
	{
		m_ForegroundLayer->DestroyPhysicsActors();
	}

	for (size_t i = 0; i < m_Entites.size(); i++)
	{
		if (m_Entites[i] != nullptr)
		{
			m_Entites[i]->DeletePhysicsActor();
		}
	}
}

sf::Vector2f Map::GetPlayerSpawnPosition() const
{
	return m_PlayerSpawnPosition;
}

void Map::ToggleLightingEditor()
{
	m_LightManager.SetShowingEditor(!m_LightManager.IsShowingEditor());
}

bool Map::StringBeginsWith(const std::string& string, const std::string& begin, std::string& extraChars)
{
	if (string.length() >= begin.length() &&
		string.substr(0, begin.length()).compare(begin) == 0)
	{
		if (string.length() > begin.length() + 1)
		{
			extraChars = string.substr(begin.length() + 1);
		}
		return true;
	}
	return false;
}

int Map::GetTilesWide() const
{
	return m_TilesWide;
}

int Map::GetTilesHigh() const
{
	return m_TilesHigh;
}

void Map::AddEntity(Entity* mob)
{
	m_Entites.push_back(mob);
}

void Map::RemoveEntity(Entity* mob)
{
	for (size_t i = 0; i < m_Entites.size(); i++)
	{
		if (m_Entites[i] == mob)
		{
			delete m_Entites[i];
			m_Entites[i] = nullptr;
			return;
		}
	}
}

void Map::AddEntityToBeRemoved(Entity* mob)
{
	for (size_t i = 0; i < m_EntitesToBeRemoved.size(); ++i)
	{
		if (m_EntitesToBeRemoved[i] == mob) return;
	}

	m_EntitesToBeRemoved.push_back(mob);

	for (size_t i = 0; i < m_Entites.size(); ++i)
	{
		if (m_Entites[i] == mob)
		{
			m_Entites[i] = nullptr;
			break;
		}
	}
}

void Map::GetNPCDataFromFile()
{
	json speechInfo;
	std::ifstream inputStream;

	inputStream.open("resources/npcs.json");
	if (inputStream.is_open())
	{
		std::string line;
		std::stringstream stringStream;

		while (std::getline(inputStream, line))
		{
			stringStream << line;
		}
		inputStream.close();

		speechInfo = json::parse(stringStream.str());

		std::vector<json> characters = speechInfo["characters"].get<std::vector<json>>();
		for (size_t i = 0; i < characters.size(); ++i)
		{
			if (speechInfo["characters"][i]["spawn_point"]["building"].get<int>() == m_BuildingIndex) // They spawn on this map
			{
				m_Entites.push_back(new AICharacter(m_World, *this, characters[i]));
			}
		}
	}
}
