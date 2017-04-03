
#include "Game\World\Tile.h"
#include "Game\enumerations.h"

#include "Apex\Physics\PhysicsActor.h"

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

std::string Tile::GetStringID() const
{
	return m_StringID;
}

void Tile::SetStringID(const std::string& stringID)
{
	m_StringID = stringID;
}

void Tile::SetPhysicsActor(apex::PhysicsActor* actor)
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

void Tile::BeginContact(apex::Contact* contact)
{
	switch (ActorID(contact->actorB->GetUserData()))
	{
	case ActorID::PLAYER:
	{
	} break;
	}
}
