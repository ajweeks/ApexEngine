#pragma once

#include "Entity.h"
#include "SpriteSheet.h"
#include "Gun.h"
#include <SFML\Graphics\Texture.hpp>
#include <SFML\Graphics\Sprite.hpp>

class Level;

class Player : public Entity
{
public:
	enum class AnimationSequence
	{
		WALKING, RUNNING
	};
	enum class PlayerTransition
	{
		CROUCH, COLOUR
	};

	Player(Level* level, sf::Vector2f initalPosition = sf::Vector2f());
	virtual ~Player();

	void Tick(sf::Time elapsed);
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states);

	void Reset();

	sf::Vector2f GetPosition() const;

private:
	void ClampPosition();
	void HandleMovement(sf::Time elapsed);
	
	static const float MAX_VEL;
	static const float FRICTION;

	bool m_IsCrouching;

	sf::Vector2f m_IntialPos;
	float m_SecondsElapsed = 0.0f;

	Level* m_Level = nullptr;
	sf::Texture m_GlowTexture;
	sf::Sprite m_GlowSprite;

	Gun m_Gun;
	SpriteSheet m_SpriteSheet;

};
