
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
#include "Bed.h"
#include "Coin.h"
#include "SolidBlock.h"

#include <JSON\json.hpp>

#include <fstream>
#include <map>

sf::Shader Map::s_OutlinedSpriteShader;

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

	for (size_t i = 0; i < m_Mobs.size(); i++)
	{
		delete m_Mobs[i];
	}
	m_Mobs.clear();
	for (size_t i = 0; i < m_MobsToBeRemoved.size(); i++)
	{
		delete m_MobsToBeRemoved[i];
	}
	m_MobsToBeRemoved.clear();

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

	delete m_BackgroundLayer;
	m_BackgroundLayer = nullptr;

	delete m_ForegroundLayer;
	m_ForegroundLayer = nullptr;

	delete m_TileSet;
	m_TileSet = nullptr;

	m_HighlightedEntity = nullptr;
	ReadNPCDataFromFile();
	ReadJSONFile();
}

void Map::ReadJSONFile()
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
		Tile::ExtraInfo tileExtraInfo{};
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
				std::string extraChars;
				if (StringBeginsWith(tileStringID, "door", extraChars))
				{
					tileExtraInfo.buildingID = stoi(extraChars);
				}
				if (StringBeginsWith(tileStringID, "exit", extraChars))
				{
				}
				if (StringBeginsWith(tileStringID, "building", extraChars))
				{
					tileExtraInfo.buildingID = stoi(extraChars);
				}
			}
		}
		Tile* tile = new Tile(ID, solid, sensor);
		if (!tileStringID.empty())
		{
			tile->SetStringID(tileStringID);
		}
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

			if (layerName.compare("midground") == 0)
			{
				for (size_t i = 0; i < tileIDs.size(); ++i)
				{
					const int tileSize = 16;
					const int x = i % layerWidth;
					const int y = i / layerWidth;
					const sf::Vector2f topLeft = sf::Vector2f(float(x * tileSize), float(y * tileSize));
					const std::string stringID = tileAtlas[tileIDs[i]]->GetStringID();
					if (stringID.compare("bed") == 0)
					{
						Bed* bed = new Bed(m_World, this, topLeft);
						m_Items.push_back(bed);
					}
					else if (stringID.compare("coin") == 0)
					{
						Coin* coin = new Coin(m_World, this, topLeft);
						m_Items.push_back(coin);
					}
					else if (StringBeginsWith(stringID, "solid_block", ex))
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
					tile->SetExtraInfo(tileAtlas[id]->GetExtraInfo());
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
				else ApexOutputDebugString("ERROR: Unhandled layer name! " + layerName + "\n");
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

	// For now, all layers must be included in map files
	assert(m_BackgroundLayer != nullptr && m_ForegroundLayer != nullptr);

	for (size_t i = 0; i < tileAtlas.size(); ++i)
	{
		delete tileAtlas[i];
	}
	tileAtlas.clear();
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
}

bool greater(std::pair<float, Entity*> lhs, std::pair<float, Entity*> rhs) { return lhs.first > rhs.first; }

/*

	First draw the background tiles
	Second, sort all items/mobs/player by their y pos
	Third draw items/mobs/player from back to front
	Fourth draw foreground elements

*/
void Map::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	m_BackgroundLayer->Draw(target, states);

	std::multimap<float, Entity*> m_SortedMidgroundEntities; // sorted by y value
	typedef std::multimap<float, Entity*>::iterator iter;
	for (size_t i = 0; i < m_Mobs.size(); i++)
	{
		if (m_Mobs[i] != nullptr)
		{
			m_SortedMidgroundEntities.emplace(m_Mobs[i]->GetPhysicsActor()->GetPosition().y, m_Mobs[i]);
		}
	}

	for (size_t i = 0; i < m_Items.size(); i++)
	{
		if (m_Items[i] != nullptr)
		{
			m_SortedMidgroundEntities.emplace(m_Items[i]->GetPhysicsActor()->GetPosition().y, m_Items[i]);
		}
	}
	m_SortedMidgroundEntities.emplace(m_World->GetPlayer()->GetPhysicsActor()->GetPosition().y, m_World->GetPlayer());

	for (iter it = m_SortedMidgroundEntities.begin(); it != m_SortedMidgroundEntities.end(); ++it)
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
	m_ForegroundLayer->CreatePhysicsActors(contactListener);

	for (size_t i = 0; i < m_Mobs.size(); i++)
	{
		m_Mobs[i]->CreatePhysicsActor(contactListener);
	}
	for (size_t i = 0; i < m_Items.size(); i++)
	{
		m_Items[i]->CreatePhysicsActor();
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

	for (size_t i = 0; i < m_Mobs.size(); i++)
	{
		if (m_Mobs[i] != nullptr)
		{
			m_Mobs[i]->DeletePhysicsActor();
		}
	}
	for (size_t i = 0; i < m_Items.size(); i++)
	{
		if (m_Items[i] != nullptr)
		{
			m_Items[i]->DeletePhysicsActor();
		}
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
