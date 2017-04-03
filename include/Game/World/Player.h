#pragma once

#include "Mob.h"
#include "Gun.h"

#include "Apex\Graphics\SpriteSheet.h"
#include "Apex\IO\KeyListener.h"

#include <SFML\Graphics\Texture.hpp>
#include <SFML\Graphics\Sprite.hpp>

class Player : public Mob, public apex::KeyListener
{
public:
	enum AnimationSequence
	{
		WALKING_DOWN, WALKING_UP, WALKING_SIDEWAYS
	};
	enum DirectionFacing
	{
		LEFT, RIGHT, UP, DOWN
	};

	Player(World& world, Map& map);
	virtual ~Player();

	void Tick(sf::Time elapsed);
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states);

	void Reset();

	sf::Vector2f GetPosition() const;
	
	int GetDoorIndexTouching() const;

	void StopMoving();

	virtual void CreatePhysicsActor() override;

	virtual void BeginContact(apex::Contact* contact) override;
	virtual void EndContact(apex::Contact* contact) override;
	virtual void PreSolve(apex::Contact* contact, bool& enableContact) override;

	virtual bool OnKeyPress(apex::Keyboard::Key key, bool keyPressed) override;
	virtual void OnKeyRelease(apex::Keyboard::Key key) override;

	virtual sf::Vector2f GetBottomMiddlePoint() override;

private:
	void ClampPosition();
	void HandleMovement(sf::Time elapsed);
	void DrawBody(sf::RenderTarget& target, sf::RenderStates states);

	static const float VEL;
	static const float MAX_VEL;
	static const float FRICTION;

	static const float WIDTH;
	static const float HEIGHT;

	DirectionFacing m_DirFacing;
	bool m_StandingStill;

	float m_SecondsElapsed = 0.0f;

	b2Fixture* m_FootFixture = nullptr;
	b2Fixture* m_BodyFixture = nullptr;

	apex::SpriteSheet m_SpriteSheet;

	int m_DoorIndexTouching = -1;
	int m_BuildingIndexToEnterNextFrame = -1;
	bool m_ExitBuildingNextFrame = false;
};
