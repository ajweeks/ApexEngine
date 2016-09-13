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
		STANDING, WALKING
	};
	enum DirectionFacing
	{
		LEFT, RIGHT
	};

	Player(World* world, Map* map);
	virtual ~Player();

	void Tick(sf::Time elapsed);
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states);

	void Reset();

	sf::Vector2f GetPosition() const;

	void StopMoving();

	virtual void CreatePhysicsActor() override;

	virtual void BeginContact(ApexContact* contact) override;
	virtual void EndContact(ApexContact* contact) override;
	virtual void PreSolve(ApexContact* contact, bool& enableContact) override;

	virtual bool OnKeyPress(ApexKeyboard::Key key, bool keyPressed) override;
	virtual void OnKeyRelease(ApexKeyboard::Key key) override;

private:
	void ClampPosition();
	void HandleMovement(sf::Time elapsed);
	void DrawShadow(sf::RenderTarget& target, sf::RenderStates states);
	void DrawBody(sf::RenderTarget& target, sf::RenderStates states);

	static const float VEL;
	static const float MAX_VEL;
	static const float FRICTION;

	DirectionFacing m_DirFacing;

	float m_SecondsElapsed = 0.0f;


	ApexSpriteSheet m_SpriteSheet;

	int m_BuildingIndexToEnterNextFrame = -1;
	bool m_ExitBuildingNextFrame = false;
};
