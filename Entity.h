#pragma once

#include <SFML\Graphics\Rect.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\RenderStates.hpp>
#include <SFML\System\Time.hpp>

class PhysicsActor;
class Level;

class Entity
{
public:
	enum class Type
	{
		PLAYER, BG_TILE, BULLET, GUN
	};

	Entity(Level* level, sf::Vector2f position, Type type, void* userPointer = nullptr);
	virtual ~Entity();

	Entity& operator=(const Entity&) = delete;
	Entity(const Entity&) = delete;

	virtual void Tick(sf::Time elapsed) = 0;
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states) = 0;

	sf::Vector2f GetPosition() const;
	sf::Vector2f GetVelocity() const;

protected:
	PhysicsActor* m_Actor;

private:
	sf::FloatRect m_BoundingBox;

};
