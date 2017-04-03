#pragma once

#include "Entity.h"

#include "Apex\IO\MouseListener.h"

#include <SFML\System\Vector2.hpp>
#include <SFML\Graphics\RectangleShape.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

class Player;
class World;
class BulletManager;
class Game;
class AmmoDrop;
class Map;

class Gun : public Entity, public apex::MouseListener
{
public:
	Gun(World& world, Map& map, sf::Vector2f position);
	virtual ~Gun();

	Gun(const Gun&) = delete;
	Gun& operator=(const Gun&) = delete;

	virtual void Tick(sf::Time elapsed) override;
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states) override;
	virtual sf::Vector2f GetBottomMiddlePoint() override;

	void Shoot();
	void Reset();

	bool Empty() const;

	void AddAmmo(AmmoDrop* ammoDrop);

	int GetBulletsInClip() const;
	int GetBulletsRemaining() const;
	int GetClipSize() const;

	// Apex Mouse Listener overrides
	virtual bool OnButtonPress(sf::Event::MouseButtonEvent buttonEvent);
	virtual void OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent);
	virtual void OnScroll(sf::Event::MouseWheelScrollEvent scrollEvent);

	float GetDirection() const;

private:
	void Reload();

	BulletManager& m_BulletManager;

	bool m_BeingHeld;

	float m_Direction;

	sf::RectangleShape m_RectShape;

	int m_BulletsInClip;
	int m_BulletsRemaining;
	int m_ClipSize;

	static const float RELOAD_SECONDS;
	sf::Time m_ReloadingTimeRemaining;

};
