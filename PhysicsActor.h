#pragma once

#include <SFML\Graphics.hpp>

class PhysicsBody;
class Level;

class PhysicsActor
{
public:
	PhysicsActor(sf::Vector2f position);
	virtual ~PhysicsActor();

	PhysicsActor(const PhysicsActor&) = delete;
	PhysicsActor& operator=(const PhysicsActor&) = delete;
	
	void Tick(sf::Time elapsed);
	void draw(sf::RenderTarget& target, sf::RenderStates states);

	void SetPosition(sf::Vector2f newPosition);
	sf::Vector2f GetPosition() const;
	void GetVelocity(sf::Vector2f newVelocity);
	sf::Vector2f GetVelocity() const;

private:
	PhysicsBody* m_Body = nullptr;
	Level* m_Level = nullptr;

};
