#pragma once

#include "ApexContactListener.h"

#include <string>

class PhysicsActor;

class Tile : public ApexContactListener
{
public:
	enum class Type
	{
		NORMAL, DOOR, EXIT, BUILDING,
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
	std::string GetStringID() const;

	void SetExtraInfo(ExtraInfo extraInfo);
	void SetStringID(const std::string& stringID);

	void SetPhysicsActor(PhysicsActor* actor);
	void DeletePhysicsActor();

	virtual void BeginContact(ApexContact* contact);
	virtual void EndContact(ApexContact* contact) {};
	virtual void PreSolve(ApexContact* contact, bool& enableContact) {};

private:
	bool m_IsSolid;
	bool m_IsSensor;
	int m_ID;
	std::string m_StringID;
	Type m_Type;
	ExtraInfo m_ExtraInfo;
	PhysicsActor* m_Actor = nullptr;

};
