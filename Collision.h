#pragma once

#include <SFML\System\Vector2.hpp>

class Manifold;
class PhysicsActor;
class BoxFixture;

class Collision
{
public:
	static void CircleVCircle(Manifold* manifold, PhysicsActor* a, PhysicsActor* b);
	static void CircleVBox(Manifold* manifold, PhysicsActor* a, PhysicsActor* b);
	static void BoxVCircle(Manifold* manifold, PhysicsActor* a, PhysicsActor* b);
	static void BoxVBox(Manifold* manifold, PhysicsActor* a, PhysicsActor* b);

private:
	static int Clip(sf::Vector2f n, float c, sf::Vector2f *face);
	static float FindAxisLeastPenetration(int *faceIndex, BoxFixture *A, BoxFixture *B);
	static void FindIncidentFace(sf::Vector2f *v, PhysicsActor *RefPoly, PhysicsActor *IncPoly, int referenceIndex);
	static bool BiasGreaterThan(float a, float b);


	Collision() = delete;

};
