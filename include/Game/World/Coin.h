#pragma once

#include "Item.h"

#include "Apex\Graphics\SpriteSheet.h"

class Coin : public Item
{
public:
	Coin(World& world, Map& map, sf::Vector2f position);
	virtual ~Coin();

	Coin& operator=(const Coin&) = delete;
	Coin(const Coin&) = delete;

	void Tick(sf::Time elapsed) override;
	void Draw(sf::RenderTarget& target, sf::RenderStates states) override;
	virtual sf::Vector2f GetBottomMiddlePoint() override;

	void BeginContact(apex::Contact* contact) override;

	virtual void CreatePhysicsActor() override;

private:
	void DrawShadow(sf::RenderTarget& target, sf::RenderStates states);

	apex::SpriteSheet m_SpriteSheet;

};