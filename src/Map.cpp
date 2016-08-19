
#include "Map.h"
#include "Layer.h"
#include "TileSet.h"
#include "TextureManager.h"
#include "Tile.h"
#include "World.h"
#include "Mob.h"
#include "Item.h"
#include "ApexMath.h"
#include "Interactable.h"
#include "PhysicsActor.h"
#include "ApexNPC.h"
#include "Layer.h"
#include "LightManager.h"

#include <JSON\json.hpp>

#include <fstream>

sf::Shader Map::s_OutlinedSpriteShader;

using json = nlohmann::json;

Map::Map(World* world, int buildingIndex, std::string directory) :
	m_LightManager(world, directory),
	m_World(world),
	m_BuildingIndex(buildingIndex)
{
	const std::string fileName = "tiles.json";
	const std::string filePath = directory + fileName;

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
		std::cout << "File \"" << filePath << "\" was empty!" << std::endl;
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
		Tile::Type tileType = Tile::Type::NORMAL;
		Tile::ExtraInfo tileExtraInfo{};
		const int ID = i - 1;
		bool solid = false;
		bool sensor = false;
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
				const std::string tileIDString = currentTile["id"].get<std::string>();
				std::string extraChars;
				if (StringBeginsWith(tileIDString, "door", extraChars))
				{
					tileExtraInfo.buildingID = stoi(extraChars);
					tileType = Tile::Type::DOOR;
				}
				if (StringBeginsWith(tileIDString, "exit", extraChars))
				{
					tileType = Tile::Type::EXIT;
				}
				if (StringBeginsWith(tileIDString, "building", extraChars))
				{
					tileType = Tile::Type::BUILDING;
					tileExtraInfo.buildingID = stoi(extraChars);
				}
				if (StringBeginsWith(tileIDString, "bed", extraChars))
				{
					tileType = Tile::Type::BED;
				}
			}
		}
		Tile* tile = new Tile(ID, solid, sensor, tileType);
		tile->SetExtraInfo(tileExtraInfo);
		tileAtlas.push_back(tile);
	}

	m_TileSet = new TileSet(TextureManager::GetTexture(TextureManager::GENERAL_TILES), tileSetTileSize, tileSetMargin, tileSetSpacing);

	m_PlayerSpawnPosition = APEX->StringToVector2f(tileMap["properties"]["player_spawn"]);
	m_PlayerSpawnPosition.x *= m_TileSet->m_TileSize;
	m_PlayerSpawnPosition.y *= m_TileSet->m_TileSize;

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
			std::vector<Tile*> tiles;
			tiles.reserve(tileIDs.size());
			for (size_t i = 0; i < tileIDs.size(); ++i)
			{
				const int id = tileIDs[i];
				Tile::Type tileType = tileAtlas[id]->GetType();
				Tile* tile = new Tile(id, tileAtlas[id]->IsSolid(), tileAtlas[id]->IsSensor(), tileType);
				tile->SetExtraInfo(tileAtlas[id]->GetExtraInfo());
				tiles.push_back(tile);
			}

			Layer* newLayer = new Layer(m_World, tiles, m_TileSet, layerName, layerVisible,
				layerOpacity, layerType, layerWidth, layerHeight);

			if (layerName.compare("background") == 0) m_BackgroundLayer = newLayer;
			else if (layerName.compare("midground") == 0) m_MidgroundLayer = newLayer;
			else if (layerName.compare("foreground") == 0) m_ForegroundLayer = newLayer;
			else ApexOutputDebugString("ERROR: Unhandled layer name! " + layerName + "\n");
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

	std::string orientation = tileMap["orientation"];
	if (orientation.compare("orthogonal") != 0)
	{
		std::cout << "Non-orthogonal map passed in! The type specified was \"" << orientation << "\"!" << std::endl;
		return;
	}

	for (size_t i = 0; i < tileAtlas.size(); ++i)
	{
		delete tileAtlas[i];
	}
	tileAtlas.clear();
}

Map::~Map()
{
	delete m_BackgroundLayer;
	delete m_MidgroundLayer;
	delete m_ForegroundLayer;

	delete m_TileSet;

	for (size_t i = 0; i < m_Mobs.size(); i++)
	{
		if (m_Mobs[i] != nullptr)
		{
			delete m_Mobs[i];
		}
	}
	m_Mobs.clear();
	for (size_t i = 0; i < m_MobsToBeRemoved.size(); ++i)
	{
		delete m_MobsToBeRemoved[i];
	}
	m_MobsToBeRemoved.clear();

	for (size_t i = 0; i < m_Items.size(); i++)
	{
		if (m_Items[i] != nullptr)
		{
			delete m_Items[i];
		}
	}
	m_Items.clear();
	for (size_t i = 0; i < m_ItemsToBeRemoved.size(); i++)
	{
		delete m_ItemsToBeRemoved[i];
	}
	m_ItemsToBeRemoved.clear();

	m_HighlightedEntity = nullptr;
}

void Map::Reset()
{
	DestroyPhysicsActors();

	for (size_t i = 0; i < m_Items.size(); i++)
	{
		delete m_Items[i];
	}
	m_Items.clear();
	for (size_t i = 0; i < m_ItemsToBeRemoved.size(); i++)
	{
		delete m_ItemsToBeRemoved[i];
	}
	m_ItemsToBeRemoved.clear();

	for (size_t i = 0; i < m_Mobs.size(); i++)
	{
		delete m_Mobs[i];
	}
	m_Mobs.clear();
	for (size_t i = 0; i < m_MobsToBeRemoved.size(); ++i)
	{
		delete m_MobsToBeRemoved[i];
	}
	m_MobsToBeRemoved.clear();

	ReadNPCDataFromFile();

	m_HighlightedEntity = nullptr;
}

void Map::LoadShaders()
{
	if (!s_OutlinedSpriteShader.loadFromFile("resources/shaders/outline_sprite.frag", sf::Shader::Fragment))
	{
		ApexOutputDebugString("\n\n\n\tCould not either load or compile outline_sprite.frag\n\n\n\n");
	}
	s_OutlinedSpriteShader.setParameter("u_color", sf::Color(255, 255, 255, 210));
}

void Map::Tick(sf::Time elapsed)
{
	for (size_t i = 0; i < m_ItemsToBeRemoved.size(); ++i)
	{
		delete m_ItemsToBeRemoved[i];
	}
	m_ItemsToBeRemoved.clear();
	
	m_BackgroundLayer->Tick(elapsed);
	m_MidgroundLayer->Tick(elapsed);
	m_ForegroundLayer->Tick(elapsed);

	for (size_t i = 0; i < m_Mobs.size(); i++)
	{
		if (m_Mobs[i] != nullptr)
		{
			m_Mobs[i]->Tick(elapsed);
		}
	}

	for (size_t i = 0; i < m_Items.size(); i++)
	{
		if (m_Items[i] != nullptr)
		{
			m_Items[i]->Tick(elapsed);
		}
	}

	m_LightManager.Tick(elapsed);

	const float minDist = 28.0f;
	float distanceToNearestEntity;
	Entity* nearestEntity = GetNearestInteractableEntityTo(m_World->GetPlayer(), distanceToNearestEntity);
	if (distanceToNearestEntity != -1.0f && distanceToNearestEntity <= minDist)
	{
		m_HighlightedEntity = nearestEntity;
	}
	else
	{
		m_HighlightedEntity = nullptr;
	}

	// Somehow the player moved away while talking with someone, clear the speech bubble
	if (m_HighlightedEntity == nullptr && m_World->IsShowingSpeechBubble())
	{
		m_World->ClearSpeechShowing();
	}
}

/*

	First draw the background tiles
	Second, sort all items/mobs/player by their y pos
	Third draw items/mobs/player from back to front

*/
void Map::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	m_BackgroundLayer->Draw(target, states);
	m_MidgroundLayer->Draw(target, states);
	m_ForegroundLayer->Draw(target, states);

	for (size_t i = 0; i < m_Mobs.size(); i++)
	{
		if (m_Mobs[i] != nullptr && m_Mobs[i] != m_HighlightedEntity)
		{
			m_Mobs[i]->Draw(target, states);
		}
	}

	for (size_t i = 0; i < m_Items.size(); i++)
	{
		if (m_Items[i] != nullptr && m_Items[i] != m_HighlightedEntity)
		{
			m_Items[i]->Draw(target, states);
		}
	}

	if (m_HighlightedEntity != nullptr)
	{
		states.shader = &s_OutlinedSpriteShader;
		m_HighlightedEntity->Draw(target, states);
		states.transform = states.Default.transform;
		states.shader = states.Default.shader;
	}

	m_LightManager.Draw(target, states);
}

Entity* Map::GetNearestInteractableEntityTo(Entity* sourceEntity, float& distance)
{
	Entity* nearestSoFar = nullptr;
	distance = -1.0f;

	const sf::Vector2f sourcePos = sourceEntity->GetPhysicsActor()->GetPosition();
	for (size_t i = 0; i < m_Mobs.size(); ++i)
	{
		Interactable* interactable = dynamic_cast<Interactable*>(m_Mobs[i]);
		if (interactable)
		{
			const sf::Vector2f otherPos = m_Mobs[i]->GetPhysicsActor()->GetPosition();
			const float currentDistance = ApexMath::Distance(sourcePos, otherPos);

			if (distance == -1.0f || currentDistance < distance)
			{
				distance = currentDistance;
				nearestSoFar = m_Mobs[i];
			}
		}
	}
	for (size_t i = 0; i < m_Items.size(); ++i)
	{
		Interactable* interactable = dynamic_cast<Interactable*>(m_Items[i]);
		if (interactable)
		{
			const sf::Vector2f otherPos = m_Items[i]->GetPhysicsActor()->GetPosition();
			const float currentDistance = ApexMath::Distance(sourcePos, otherPos);

			if (distance == -1.0f || currentDistance < distance)
			{
				distance = currentDistance;
				nearestSoFar = m_Items[i];
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

int Map::GetTileSize() const
{
	return m_TileSet->m_TileSize;
}

void Map::CreatePhysicsActors(ApexContactListener* contactListener)
{
	m_BackgroundLayer->CreatePhysicsActors(contactListener);
	m_MidgroundLayer->CreatePhysicsActors(contactListener);
	m_ForegroundLayer->CreatePhysicsActors(contactListener);

	for (size_t i = 0; i < m_Mobs.size(); i++)
	{
		m_Mobs[i]->CreatePhysicsActor(contactListener);
	}
}

void Map::DestroyPhysicsActors()
{
	m_BackgroundLayer->DestroyPhysicsActors();
	m_MidgroundLayer->DestroyPhysicsActors();
	m_ForegroundLayer->DestroyPhysicsActors();

	for (size_t i = 0; i < m_Mobs.size(); i++)
	{
		m_Mobs[i]->DeletePhysicsActor();
	}
}

sf::Vector2f Map::GetPlayerSpawnPosition() const
{
	return m_PlayerSpawnPosition;
}

void Map::ToggleLightingEditor()
{
	m_LightManager.ToggleShowingEditor();
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

void Map::AddMob(Mob* mob)
{
	m_Mobs.push_back(mob);
}

void Map::RemoveMob(Mob* mob)
{
	for (size_t i = 0; i < m_Mobs.size(); i++)
	{
		if (m_Mobs[i] == mob)
		{
			delete m_Mobs[i];
			m_Mobs[i] = nullptr;
			return;
		}
	}
}

void Map::AddMobToBeRemoved(Mob* mob)
{
	for (size_t i = 0; i < m_MobsToBeRemoved.size(); ++i)
	{
		if (m_MobsToBeRemoved[i] == mob) return;
	}

	m_MobsToBeRemoved.push_back(mob);

	for (size_t i = 0; i < m_Items.size(); ++i)
	{
		if (m_Mobs[i] == mob)
		{
			m_Mobs[i] = nullptr;
			break;
		}
	}
}

void Map::AddItem(Item* item)
{
	m_Items.push_back(item);
}

void Map::RemoveItem(Item* item)
{
	for (size_t i = 0; i < m_Items.size(); i++)
	{
		if (m_Items[i] == item)
		{
			delete m_Items[i];
			m_Items[i] = nullptr;
			return;
		}
	}
}

void Map::AddItemToBeRemoved(Item* item)
{
	for (size_t i = 0; i < m_ItemsToBeRemoved.size(); ++i)
	{
		if (m_ItemsToBeRemoved[i] == item) return;
	}

	m_ItemsToBeRemoved.push_back(item);

	for (size_t i = 0; i < m_Items.size(); ++i)
	{
		if (m_Items[i] == item)
		{
			m_Items[i] = nullptr;
			break;
		}
	}
}

void Map::ReadNPCDataFromFile()
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
				m_Mobs.push_back(new ApexNPC(m_World, this, characters[i]));
			}
		}
	}
}
