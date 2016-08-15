#pragma once

#include "Entity.h"
#include "ApexMouseListener.h"

#include <SFML\System\Vector2.hpp>
#include <SFML\Graphics\RectangleShape.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

class Player;
class World;
class BulletManager;
class Game;
class AmmoDrop;

class Gun : public Entity, public ApexMouseListener
{
public:
	Gun(World* world, sf::Vector2f position);
	virtual ~Gun();

	Gun(const Gun&) = delete;
	Gun& operator=(const Gun&) = delete;

	virtual void Tick(sf::Time elapsed);
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states);

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

	bool m_BeingHeld;
	World* m_World = nullptr;
	BulletManager* m_BulletManager = nullptr;

	float m_Direction;

	sf::RectangleShape m_RectShape;

	int m_BulletsInClip;
	int m_BulletsRemaining;
	int m_ClipSize;

	static const float RELOAD_SECONDS;
	sf::Time m_ReloadingTimeRemaining;

};
