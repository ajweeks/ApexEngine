#pragma once

#include "Mob.h"
#include "ApexSpriteSheet.h"
#include "Gun.h"
#include "ApexKeyListener.h"

#include <SFML\Graphics\Texture.hpp>
#include <SFML\Graphics\Sprite.hpp>

class World;
class Map;

class Player : public Mob, public ApexKeyListener
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

	virtual void BeginContact(ApexContact* contact) override;
	virtual void EndContact(ApexContact* contact) override;
	virtual void PreSolve(ApexContact* contact, bool& enableContact) override;

	virtual bool OnKeyPress(ApexKeyboard::Key key, bool keyPressed) override;
	virtual void OnKeyRelease(ApexKeyboard::Key key) override;

	virtual sf::Vector2f GetBottomMiddlePoint() override;

private:
	void ClampPosition();
	void HandleMovement(sf::Time elapsed);
	void DrawShadow(sf::RenderTarget& target, sf::RenderStates states);
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

	ApexSpriteSheet m_SpriteSheet;

	int m_DoorIndexTouching = -1;
	int m_BuildingIndexToEnterNextFrame = -1;
	bool m_ExitBuildingNextFrame = false;
};
