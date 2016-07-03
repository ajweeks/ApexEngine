#pragma once

#include "Entity.h"
#include <SFML\System\Vector2.hpp>
#include <SFML\Graphics\RectangleShape.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\RenderStates.hpp>

class Player;
class Level;
class BulletManager;
class Game;

class Gun : public Entity
{
public:
	Gun(Level* level, sf::Vector2f position, Player* playerHolding = nullptr);
	virtual ~Gun();

	Gun(const Gun&) = delete;
	Gun& operator=(const Gun&) = delete;

	virtual void Tick(sf::Time elapsed);
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states);

	void Shoot();
	void Reset();

private:
	bool m_BeingHeld;
	Player* m_PlayerHolding = nullptr;
	Level* m_Level = nullptr;
	BulletManager* m_BulletManager = nullptr;

	float m_Direction;

	sf::RectangleShape m_RectShape;

	int m_ClipSize;
	int m_BulletsRemaining;
	int m_ClipsRemaining;

};
