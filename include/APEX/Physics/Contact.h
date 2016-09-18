#pragma once

class b2Fixture;

namespace apex
{
	class PhysicsActor;
	
	struct Contact
	{
		// Actor/fixture A is the *other* object
		PhysicsActor* actorA;
		b2Fixture* fixtureA;
		// Actor/fixture B is the contact listener
		PhysicsActor* actorB;
		b2Fixture* fixtureB;
		// Contact normal?
	};
} // namespace apex
