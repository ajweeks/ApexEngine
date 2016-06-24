#pragma once

#include <SFML\Graphics.hpp>

class Level;
class Fixture;

class PhysicsActor
{
public:
	enum class Type
	{
		STATIC, DYNAMIC //, KINEMATIC?
	};

	PhysicsActor(sf::Vector2f centerPos, Type type, Level* level);
	virtual ~PhysicsActor();

	PhysicsActor(const PhysicsActor&) = delete;
	PhysicsActor& operator=(const PhysicsActor&) = delete;
	
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

	void SetSolid(bool solid);
	bool IsSolid() const;
	
	bool IsOverlapping(PhysicsActor* otherActor);
	Type GetType() const;

	float GetRestitution() const;

private:
	sf::Vector2f m_Pos;
	sf::Vector2f m_Vel;
	bool m_Solid;
	Type m_Type;

	sf::RectangleShape m_DebugXAxisRect;
	sf::RectangleShape m_DebugYAxisRect;

	Level* m_Level = nullptr;

	std::vector<Fixture*> m_Fixtures;

	int m_UserData;
	void* m_UserPointer = nullptr;

};
