#pragma once

class PhysicsActor;
class b2Fixture;

struct ApexContact
{
	// Actor/fixture A is the *other* object
	PhysicsActor* actorA;
	b2Fixture* fixtureA;
	// Actor/fixture B is the contact listener
	PhysicsActor* actorB;
	b2Fixture* fixtureB;
	// Contact normal?
};
