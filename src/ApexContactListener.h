#pragma once

#include "ApexContact.h"

class ApexContactListener
{
public:
	ApexContactListener();
	virtual ~ApexContactListener();

	ApexContactListener(const ApexContactListener&) = delete;
	ApexContactListener& operator=(const ApexContactListener&) = delete;

	virtual void BeginContact(ApexContact* contact) {};
	virtual void EndContact(ApexContact* contact) {};
	virtual void PreSolve(ApexContact* contact, bool& enableContact) {};

private:

};
