#pragma once

#include "Entity.h"
#include "Level.h"

class PhysicsActor;

class Player : public Entity
{
public:
	Player(Level* level, sf::Vector2f initalPosition = sf::Vector2f());
	virtual ~Player();

	void Tick(sf::Time elapsed);
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states);

	void Reset();

	sf::Vector2f GetPosition() const;

private:
	void ClampPosition();

	PhysicsActor* m_Actor;

	Level* m_Level;
	sf::Texture m_GlowTexture;
	sf::Sprite m_GlowSprite;

};
