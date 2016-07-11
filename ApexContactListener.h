#pragma once

class PhysicsActor;

class ApexContactListener
{
public:
	ApexContactListener();
	virtual ~ApexContactListener();

	ApexContactListener(const ApexContactListener&) = delete;
	ApexContactListener& operator=(const ApexContactListener&) = delete;

	virtual void BeginContact(PhysicsActor* thisActor, PhysicsActor* otherActor) {};
	virtual void EndContact(PhysicsActor* thisActor, PhysicsActor* otherActor) {};
	virtual void PreSolve(PhysicsActor* thisActor, PhysicsActor* otherActor, bool& enableContact) {};

private:

};
