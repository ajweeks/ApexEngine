#pragma once

#include "Entity.h"
#include "Level.h"

class PhysicsActor;
class Gun;

class Player : public Entity
{
public:
	Player(Level* level, sf::Vector2f initalPosition = sf::Vector2f());
	virtual ~Player();

	void Tick(sf::Time elapsed);
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states);

	void Reset();

	sf::Vector2f GetPosition() const;
	//sf::Vector2f GetSize() const;

private:
	void ClampPosition();

	sf::Vector2f m_IntialPos;
	float m_Elapsed = 0.0f;

	Level* m_Level = nullptr;
	sf::Texture m_GlowTexture;
	sf::Sprite m_GlowSprite;

	Gun* m_Gun = nullptr;

};
