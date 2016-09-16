
#include "Tile.h"
#include "PhysicsActor.h"
#include "enumerations.h"

Tile::Tile(int ID, bool isSolid, bool isSensor)
	: m_ID(ID), m_IsSolid(isSolid),m_IsSensor(isSensor)
{
}

Tile::~Tile()
{
	if (m_Actor != nullptr)
	{
		delete m_Actor;
	}
}

int Tile::GetID() const
{
	return m_ID;
}

bool Tile::IsSolid() const
{
	return m_IsSolid;
}

bool Tile::IsSensor() const
{
	return m_IsSensor;
}

int Tile::GetBuildingID() const
{
	return m_ExtraInfo.buildingID;
}

Tile::ExtraInfo Tile::GetExtraInfo() const
{
	return m_ExtraInfo;
}

std::string Tile::GetStringID() const
{
	return m_StringID;
}

void Tile::SetExtraInfo(ExtraInfo extraInfo)
{
	m_ExtraInfo = extraInfo;
}

void Tile::SetStringID(const std::string& stringID)
{
	m_StringID = stringID;
}

void Tile::SetPhysicsActor(PhysicsActor* actor)
{
	m_Actor = actor;
	m_Actor->AddContactListener(this);
}

void Tile::DeletePhysicsActor()
{
	if (m_Actor != nullptr)
	{
		delete m_Actor;
		m_Actor	= nullptr;
	}
}

void Tile::BeginContact(ApexContact* contact)
{
	switch (contact->actorB->GetUserData())
	{
	case ActorID::PLAYER:
	{
	} break;
	}
}
