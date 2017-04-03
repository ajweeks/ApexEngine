#pragma once

#include "Apex\Physics\ContactListener.h"

#include <SFML\System\Vector2.hpp>

#include <string>

namespace apex
{
	class PhysicsActor;
}

class Tile : public apex::ContactListener
{
public:
	Tile(int ID, bool isSolid, bool isSensor = false);
	virtual ~Tile();

	Tile(const Tile&) = delete;
	Tile& operator=(const Tile&)  = delete;

	int GetID() const;
	bool IsSolid() const;
	bool IsSensor() const;
	std::string GetStringID() const;

	void SetStringID(const std::string& stringID);

	void SetPhysicsActor(apex::PhysicsActor* actor);
	void DeletePhysicsActor();

	virtual void BeginContact(apex::Contact* contact);
	virtual void EndContact(apex::Contact* contact) {};
	virtual void PreSolve(apex::Contact* contact, bool& enableContact) {};

private:
	bool m_IsSolid;
	bool m_IsSensor;
	int m_ID;
	std::string m_StringID;
	apex::PhysicsActor* m_Actor = nullptr;

};
