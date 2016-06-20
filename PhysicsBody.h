#pragma once

#include <SFML\Graphics.hpp>

class Fixture;

class PhysicsBody
{
public:
	PhysicsBody(sf::Vector2f position);
	virtual ~PhysicsBody();

	PhysicsBody(const PhysicsBody&) = delete;
	PhysicsBody& operator=(const PhysicsBody&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

	void SetPosition(sf::Vector2f newPosition);
	sf::Vector2f GetPosition() const;

	void SetVelocity(sf::Vector2f newVelocity);
	sf::Vector2f GetVelocity() const;

	void AddFixture(Fixture* newFixture);
	std::vector<Fixture*> GetFixtures() const;

	void SetUserData(int userData);
	int GetUserData() const;

	void SetUserPointer(void* userPointer);
	void* GetUserPointer() const;

private:
	sf::Vector2f m_Pos;
	sf::Vector2f m_Vel;

	std::vector<Fixture*> m_Fixtures;

	int m_UserData;
	void* m_UserPointer = nullptr;

};
