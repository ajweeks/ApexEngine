#pragma once

#include "Entity.h"

#include <SFML\Graphics\VertexArray.hpp>
#include <SFML\Graphics\Text.hpp>

class Building : public Entity
{
public:
	Building(World& world, Map& map, sf::Vector2f position, sf::Texture* texture, sf::Vector2u size, sf::Vector2u tileXY, sf::Vector2f doorPos = sf::Vector2f());
	virtual ~Building();

	Building(const Building&) = delete;
	Building& operator=(const Building&) = delete;

	virtual void Tick(sf::Time elapsed) override;
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states) override;

	virtual void CreatePhysicsActor() override;
	virtual sf::Vector2f GetBottomMiddlePoint() override;

	b2Fixture* GetDoorFixture();

	size_t GetIndex() const;

	void SetDoorLocked(bool locked);
	bool IsDoorLocked() const;

private:
	int m_TilesWide;
	int m_TilesHigh;

	bool m_DoorLocked = false;

	sf::Vector2f m_DoorPosition; // Where the door is relative to us

	b2Fixture* m_MainFixture = nullptr;
	b2Fixture* m_DoorFixture = nullptr;

	sf::Text m_DoorHintText;

	size_t m_Index;

	sf::Texture* m_Texture;
	sf::VertexArray m_Verticies;

};
