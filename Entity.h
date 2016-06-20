#pragma once

#include <SFML\Graphics.hpp>

class Entity
{
public:
	enum class Type
	{
		PLAYER // ...
	};

	Entity(Type type);
	virtual ~Entity();

	Entity& operator=(const Entity&) = delete;
	Entity(const Entity&) = delete;

	virtual void Tick(sf::Time elapsed) = 0;
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states) = 0;

	sf::Vector2f GetVelocity() const;

protected:
	sf::Vector2f m_Vel;

private:
	Type m_Type;
	sf::FloatRect m_BoundingBox;

};
