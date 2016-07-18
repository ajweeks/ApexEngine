#pragma once

#include "Entity.h"
#include "ApexMouseListener.h"

#include <SFML\System\Vector2.hpp>
#include <SFML\Graphics\RectangleShape.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

class Player;
class Level;
class BulletManager;
class Game;

class Gun : public Entity, public ApexMouseListener
{
public:
	Gun(Level* level, sf::Vector2f position);
	virtual ~Gun();

	Gun(const Gun&) = delete;
	Gun& operator=(const Gun&) = delete;

	virtual void Tick(sf::Time elapsed);
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states);

	void Shoot();
	void Reset();

	// Apex Mouse Listener overrides
	virtual bool OnButtonPress(sf::Event::MouseButtonEvent buttonEvent);
	virtual void OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent);
	virtual void OnScroll(sf::Event::MouseWheelScrollEvent scrollEvent);

	float GetDirection() const;

private:
	bool m_BeingHeld;
	Level* m_Level = nullptr;
	BulletManager* m_BulletManager = nullptr;

	float m_Direction;

	sf::RectangleShape m_RectShape;

	int m_ClipSize;
	int m_BulletsRemaining;
	int m_ClipsRemaining;

};
