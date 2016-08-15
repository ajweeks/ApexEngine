#pragma once

#include "ApexContactListener.h"

class PhysicsActor;

class Tile : ApexContactListener
{
public:
	Tile(int ID, bool isSolid, int doorID = -1);
	virtual ~Tile();

	Tile(const Tile&) = delete;
	Tile& operator=(const Tile&)  = delete;

	int GetID() const;
	bool IsSolid() const;
	bool IsDoorTile() const;
	int GetDoorID() const;

	void SetPhysicsActor(PhysicsActor* actor);

	virtual void BeginContact(PhysicsActor* thisActor, PhysicsActor* otherActor);
	virtual void EndContact(PhysicsActor* thisActor, PhysicsActor* otherActor) {};
	virtual void PreSolve(PhysicsActor* thisActor, PhysicsActor* otherActor, bool& enableContact) {};

private:
	bool m_IsSolid;
	int m_ID;
	int m_DoorID; // If not -1 then this tile is a door to a building with this ID
	PhysicsActor* m_Actor = nullptr;

};
