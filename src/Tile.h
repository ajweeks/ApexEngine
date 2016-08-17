#pragma once

#include "ApexContactListener.h"

class PhysicsActor;

class Tile : ApexContactListener
{
public:
	enum class Type
	{
		NORMAL, DOOR, EXIT, BUILDING, BED,
		NONE
	};
	struct ExtraInfo
	{
		int buildingID = -1;
	};

	Tile(int ID, bool isSolid, bool isSensor = false, Type type = Type::NORMAL);
	virtual ~Tile();

	Tile(const Tile&) = delete;
	Tile& operator=(const Tile&)  = delete;

	int GetID() const;
	bool IsSolid() const;
	bool IsSensor() const;
	int GetBuildingID() const;
	Type GetType() const;
	ExtraInfo GetExtraInfo() const;

	void SetExtraInfo(ExtraInfo extraInfo);

	void SetPhysicsActor(PhysicsActor* actor);
	void DeletePhysicsActor();

	virtual void BeginContact(PhysicsActor* thisActor, PhysicsActor* otherActor);
	virtual void EndContact(PhysicsActor* thisActor, PhysicsActor* otherActor) {};
	virtual void PreSolve(PhysicsActor* thisActor, PhysicsActor* otherActor, bool& enableContact) {};

private:
	bool m_IsSolid;
	bool m_IsSensor;
	int m_ID;
	Type m_Type;
	ExtraInfo m_ExtraInfo;
	PhysicsActor* m_Actor = nullptr;

};
