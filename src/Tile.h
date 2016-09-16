#pragma once

#include "ApexContactListener.h"
#include <SFML\System\Vector2.hpp>

#include <string>

class PhysicsActor;

class Tile : public ApexContactListener
{
public:
	struct ExtraInfo
	{
		int buildingID = -1;
		sf::Vector2f doorPos;
	};

	Tile(int ID, bool isSolid, bool isSensor = false);
	virtual ~Tile();

	Tile(const Tile&) = delete;
	Tile& operator=(const Tile&)  = delete;

	int GetID() const;
	bool IsSolid() const;
	bool IsSensor() const;
	int GetBuildingID() const;
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
	ExtraInfo m_ExtraInfo;
	PhysicsActor* m_Actor = nullptr;

};
