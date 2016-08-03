#pragma once

#include "Entity.h"
#include "ApexSpriteSheet.h"
#include "Gun.h"
#include "ApexKeyListener.h"

#include <SFML\Graphics\Texture.hpp>
#include <SFML\Graphics\Sprite.hpp>

class Level;

class Player : public Entity, public ApexKeyListener
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

	Player(Level* level);
	virtual ~Player();

	void Tick(sf::Time elapsed);
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states);

	void Reset();

	sf::Vector2f GetPosition() const;

	void StopMoving();

	virtual void BeginContact(PhysicsActor* thisActor, PhysicsActor* otherActor) override;
	virtual void EndContact(PhysicsActor* thisActor, PhysicsActor* otherActor) override;
	virtual void PreSolve(PhysicsActor* thisActor, PhysicsActor* otherActor, bool& enableContact) override;

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

	sf::Vector2f m_IntialPos;
	float m_SecondsElapsed = 0.0f;

	Level* m_Level = nullptr;

	ApexSpriteSheet m_SpriteSheet;
};
