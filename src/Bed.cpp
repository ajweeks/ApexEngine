
#include "Bed.h"
#include "TextureManager.h"
#include "PhysicsActor.h"

Bed::Bed(World* world, Map* map, sf::Vector2f position) :
	Item(world, map, position + sf::Vector2f(WIDTH * 16 / 2.0f, HEIGHT * 16 / 2.0f), ActorID::BED, this, b2BodyType::b2_staticBody)
{
	m_Texture = TextureManager::GetTexture(TextureManager::GENERAL_TILES);
	
	m_Verticies.setPrimitiveType(sf::Quads);
	m_Verticies.resize((WIDTH * HEIGHT) * 4);

	for (int x = 0; x < WIDTH; x++)
	{
		for (int y = 0; y < HEIGHT; y++)
		{
			const int tileIndex = (x + y * WIDTH);
			const int tileSize = 16;
			sf::Vertex* currentQuad = &m_Verticies[tileIndex * 4];
			currentQuad[0].position = position + sf::Vector2f(float(x * tileSize), float(y * tileSize));
			currentQuad[1].position = position + sf::Vector2f(float((x + 1) * tileSize), float(y * tileSize));
			currentQuad[2].position = position + sf::Vector2f(float((x + 1) * tileSize), float((y + 1) * tileSize));
			currentQuad[3].position = position + sf::Vector2f(float(x * tileSize), float((y + 1) * tileSize));
			
			const int tileSrcX = 8 + x;
			const int tileSrcY = 4 + y;
			currentQuad[0].texCoords = sf::Vector2f(float(tileSrcX * tileSize), float(tileSrcY * tileSize));
			currentQuad[1].texCoords = sf::Vector2f(float((tileSrcX + 1) * tileSize), float(tileSrcY * tileSize));
			currentQuad[2].texCoords = sf::Vector2f(float((tileSrcX + 1) * tileSize), float((tileSrcY + 1) * tileSize));
			currentQuad[3].texCoords = sf::Vector2f(float(tileSrcX * tileSize), float((tileSrcY + 1) * tileSize));
		}
	}
}

Bed::~Bed()
{
}

void Bed::Tick(sf::Time elapsed)
{
}

void Bed::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	states.texture = m_Texture;
	target.draw(m_Verticies, states);
}

void Bed::CreatePhysicsActor()
{
	Item::CreatePhysicsActor();
	m_Actor->AddBoxFixture(WIDTH * 16, HEIGHT * 16);
}
