
#include "Tile.h"
#include "PhysicsActor.h"
#include "enumerations.h"

Tile::Tile(int ID, bool isSolid, int doorID)
	: m_ID(ID), m_IsSolid(isSolid), m_DoorID(doorID)
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

bool Tile::IsDoorTile() const
{
	return m_DoorID != -1;
}

int Tile::GetDoorID() const
{
	return m_DoorID;
}

void Tile::SetPhysicsActor(PhysicsActor* actor)
{
	m_Actor = actor;
	m_Actor->AddContactListener(this);
}

void Tile::BeginContact(PhysicsActor* thisActor, PhysicsActor* otherActor)
{
	switch (otherActor->GetUserData())
	{
	case ActorID::PLAYER:
	{
		if (IsDoorTile())
		{
		}
	} break;
	}
}
