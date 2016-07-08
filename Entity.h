#pragma once

#include <SFML\Graphics\Sprite.hpp>
#include <SFML\Graphics\Texture.hpp>
#include <SFML\Graphics\Rect.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\RenderStates.hpp>
#include <SFML\System\Time.hpp>

class PhysicsActor;
class Level;

class Entity
{
public:
	enum class ActorID
	{
		PLAYER, BG_TILE, BULLET, GUN
	};

	Entity(Level* level, sf::Vector2f position, ActorID id, void* userPointer = nullptr);
	virtual ~Entity();

	Entity& operator=(const Entity&) = delete;
	Entity(const Entity&) = delete;

	virtual void Tick(sf::Time elapsed) = 0;
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states) = 0;

	PhysicsActor* GetPhysicsActor() const;

protected:
	PhysicsActor* m_Actor;
	static sf::Sprite* m_ShadowSprite;
	static sf::Texture* m_ShadowTexture;

private:
	sf::FloatRect m_BoundingBox;
	static unsigned int m_InstanceCount;

};
