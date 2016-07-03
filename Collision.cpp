
#include "Collision.h"
#include "PhysicsActor.h"
#include "CircleFixture.h"
#include "BoxFixture.h"
#include "Manifold.h"
#include "ApexMath.h"

#include <exception>

void Collision::CircleVCircle(Manifold* manifold, PhysicsActor* a, PhysicsActor* b)
{
	CircleFixture* A = reinterpret_cast<CircleFixture*>(a->GetFixture());
	CircleFixture* B = reinterpret_cast<CircleFixture*>(b->GetFixture());

	if (A == nullptr || B == nullptr)
	{
		throw std::invalid_argument("Either A or B did not have a valid fixture!");
		return;
	}

	sf::Vector2f normal = b->GetPosition() - a->GetPosition();

	float dist_sqr = ApexMath::LengthSqr(normal);
	float radius = A->GetRadius() + B->GetRadius();

	if (dist_sqr >= radius * radius)
	{
		manifold->SetContactCount(0);
		return;
	}

	float distance = std::sqrt(dist_sqr);

	manifold->SetContactCount(1);

	if (distance == 0.0f)
	{
		manifold->SetPenetration(A->GetRadius());
		manifold->SetNormal(sf::Vector2f(1, 0));
		manifold->SetContact(0, a->GetPosition());
	}
	else
	{
		manifold->SetPenetration(radius - distance);
		manifold->SetNormal(normal / distance);
		manifold->SetContact(0, manifold->GetNormal() * A->GetRadius() + a->GetPosition());
	}
}

void Collision::CircleVBox(Manifold* manifold, PhysicsActor* a, PhysicsActor* b)
{
	CircleFixture *A = reinterpret_cast<CircleFixture *> (a->GetFixture());
	BoxFixture *B = reinterpret_cast<BoxFixture *>(b->GetFixture());

	if (A == nullptr || B == nullptr)
	{
		throw std::invalid_argument("Either A or B did not have a valid fixture!");
		return;
	}

	manifold->SetContactCount(0);

	// Transform circle center to Polygon model space
	sf::Vector2f center = a->GetPosition();
	center = center - b->GetPosition();

	// Find edge with minimum penetration
	// Exact concept as using support points in Polygon vs Polygon
	float separation = -FLT_MAX;
	int faceNormal = 0;
	std::vector<sf::Vector2f> bNormals = B->GetNormals();
	std::vector<sf::Vector2f> bVerticies = B->GetVerticies();
	for (int i = 0; i < 4; ++i)
	{
		float s = ApexMath::Dot(bNormals[i], center - bVerticies[i]);

		if (s > A->GetRadius())
			return;

		if (s > separation)
		{
			separation = s;
			faceNormal = i;
		}
	}

	// Grab face's vertices
	sf::Vector2f v1 = bVerticies[faceNormal];
	int i2 = faceNormal + 1 < int(B->GetVertexCount()) ? faceNormal + 1 : 0;
	sf::Vector2f v2 = bVerticies[i2];

	// Check to see if center is within polygon
	if (separation < 0.001)
	{
		manifold->SetContactCount(1);
		manifold->SetNormal(-(bNormals[faceNormal]));
		manifold->SetContact(0, manifold->GetNormal() * A->GetRadius() + a->GetPosition());
		manifold->SetPenetration(A->GetRadius());
		return;
	}

	// Determine which voronoi region of the edge center of circle lies within
	float dot1 = ApexMath::Dot(center - v1, v2 - v1);
	float dot2 = ApexMath::Dot(center - v2, v1 - v2);
	manifold->SetPenetration(A->GetRadius() - separation);

	// Closest to v1
	if (dot1 <= 0.0f)
	{
		if (ApexMath::DistSqr(center, v1) > A->GetRadius() * A->GetRadius())
			return;

		manifold->SetContactCount(1);
		sf::Vector2f n = v1 - center;
		n = ApexMath::Normalize(n);
		manifold->SetNormal(n);
		v1 = v1 + b->GetPosition();
		manifold->SetContact(0, v1);
	}

	// Closest to v2
	else if (dot2 <= 0.0f)
	{
		if (ApexMath::DistSqr(center, v2) > A->GetRadius() * A->GetRadius())
			return;

		manifold->SetContactCount(1);
		sf::Vector2f n = v2 - center;
		v2 =v2 + b->GetPosition();
		manifold->SetContact(0, v2);
		n = ApexMath::Normalize(n);
		manifold->SetNormal(n);
	}

	// Closest to face
	else
	{
		sf::Vector2f n = bNormals[faceNormal];
		if (ApexMath::Dot(center - v1, n) > A->GetRadius())
			return;

		manifold->SetNormal(-n);
		manifold->SetContact(0, manifold->GetNormal() * A->GetRadius() + a->GetPosition());
		manifold->SetContactCount(1);
	}
}

void Collision::BoxVCircle(Manifold* manifold, PhysicsActor* a, PhysicsActor* b)
{
	CircleVBox(manifold, b, a);
	manifold->SetNormal(-manifold->GetNormal());
}

//float Collision::FindAxisLeastPenetration(int *faceIndex, BoxFixture *A, BoxFixture *B)
//{
	//float bestDistance = -FLT_MAX;
	//int bestIndex;

	//const int vertexCount = A->GetVertexCount();
	//std::vector<sf::Vector2f> normalsA = A->GetNormals();
	//for (int i = 0; i < vertexCount; ++i)
	//{
	//	// Retrieve a face normal from A
	//	sf::Vector2f n = normalsA[i];
	//	//sf::Vector2f nw = A->u * n;

	//	// Transform face normal into B's model space
	//	//Mat2 buT = B->u.Transpose();
	//	//n = buT * nw;

	//	// Retrieve support point from B along -n
	//	sf::Vector2f s = B->GetSupport(-n);

	//	// Retrieve vertex on face from A, transform into
	//	// B's model space
	//	sf::Vector2f v = A->m_vertices[i];
	//	v = A->u * v + A->body->position;
	//	v -= B->body->position;
	//	v = buT * v;

	//	// Compute penetration distance (in B's model space)
	//	float d = Dot(n, s - v);

	//	// Store greatest distance
	//	if (d > bestDistance)
	//	{
	//		bestDistance = d;
	//		bestIndex = i;
	//	}
	//}

	//*faceIndex = bestIndex;
	//return bestDistance;
//}

void Collision::FindIncidentFace(sf::Vector2f *v, PhysicsActor *RefPoly, PhysicsActor *IncPoly, int referenceIndex)
{
//	sf::Vector2f referenceNormal = RefPoly->m_normals[referenceIndex];
//
//	// Calculate normal in incident's frame of reference
//	referenceNormal = RefPoly->u * referenceNormal; // To world space
//	referenceNormal = IncPoly->u.Transpose() * referenceNormal; // To incident's model space
//
//																// Find most anti-normal face on incident polygon
//	int incidentFace = 0;
//	float minDot = FLT_MAX;
//	for (uint32 i = 0; i < IncPoly->m_vertexCount; ++i)
//	{
//		float dot = ApexMath::Dot(referenceNormal, IncPoly->m_normals[i]);
//		if (dot < minDot)
//		{
//			minDot = dot;
//			incidentFace = i;
//		}
//	}
//
//	// Assign face vertices for incidentFace
//	v[0] = IncPoly->u * IncPoly->m_vertices[incidentFace] + IncPoly->body->position;
//	incidentFace = incidentFace + 1 >= (int32)IncPoly->m_vertexCount ? 0 : incidentFace + 1;
//	v[1] = IncPoly->u * IncPoly->m_vertices[incidentFace] + IncPoly->body->position;
}


int Collision::Clip(sf::Vector2f n, float c, sf::Vector2f *face)
{
//	uint32 sp = 0;
//	sf::Vector2f out[2] = {
//		face[0],
//		face[1]
//	};
//
//	// Retrieve distances from each endpoint to the line
//	// d = ax + by - c
//	real d1 = Dot(n, face[0]) - c;
//	real d2 = Dot(n, face[1]) - c;
//
//	// If negative (behind plane) clip
//	if (d1 <= 0.0f) out[sp++] = face[0];
//	if (d2 <= 0.0f) out[sp++] = face[1];
//
//	// If the points are on different sides of the plane
//	if (d1 * d2 < 0.0f) // less than to ignore -0.0f
//	{
//		// Push interesection point
//		real alpha = d1 / (d1 - d2);
//		out[sp] = face[0] + alpha * (face[1] - face[0]);
//		++sp;
//	}
//
//	// Assign our new converted values
//	face[0] = out[0];
//	face[1] = out[1];
//
//	assert(sp != 3);
//
//	return sp;
	return 0;
}

bool Collision::BiasGreaterThan(float a, float b)
{
	const float k_biasRelative = 0.95f;
	const float k_biasAbsolute = 0.01f;
	return a >= b * k_biasRelative + a * k_biasAbsolute;
}

void Collision::BoxVBox(Manifold* manifold, PhysicsActor* a, PhysicsActor* b)
{
	//BoxFixture *A = reinterpret_cast<BoxFixture *>(a->GetFixture());
	//BoxFixture *B = reinterpret_cast<BoxFixture *>(b->GetFixture());

	//if (A == nullptr || B == nullptr)
	//{
	//	throw std::invalid_argument("Either A or B did not have a valid fixture!");
	//	return;
	//}

	//manifold->SetContactCount(0);

	//// Check for a separating axis with A's face planes
	//int faceA;
	//float penetrationA = FindAxisLeastPenetration(&faceA, A, B);
	//if (penetrationA >= 0.0f)
	//	return;

	//// Check for a separating axis with B's face planes
	//int faceB;
	//float penetrationB = FindAxisLeastPenetration(&faceB, B, A);
	//if (penetrationB >= 0.0f)
	//	return;

	//int referenceIndex;
	//bool flip; // Always point from a to b

	//BoxFixture *RefPoly; // Reference
	//BoxFixture *IncPoly; // Incident

	//					   // Determine which shape contains reference face
	//if (BiasGreaterThan(penetrationA, penetrationB))
	//{
	//	RefPoly = A;
	//	IncPoly = B;
	//	referenceIndex = faceA;
	//	flip = false;
	//}

	//else
	//{
	//	RefPoly = B;
	//	IncPoly = A;
	//	referenceIndex = faceB;
	//	flip = true;
	//}
//
//	// World space incident face
//	sf::Vector2f incidentFace[2];
//	FindIncidentFace(incidentFace, RefPoly, IncPoly, referenceIndex);
//
//	//        y
//	//        ^  ->n       ^
//	//      +---c ------posPlane--
//	//  x < | i |\
//	  //      +---+ c-----negPlane--
////             \       v
////              r
////
////  r : reference face
////  i : incident poly
////  c : clipped point
////  n : incident normal
//
//// Setup reference face vertices
//	sf::Vector2f v1 = RefPoly->m_vertices[referenceIndex];
//	referenceIndex = referenceIndex + 1 == RefPoly->m_vertexCount ? 0 : referenceIndex + 1;
//	sf::Vector2f v2 = RefPoly->m_vertices[referenceIndex];
//
//	// Transform vertices to world space
//	v1 = RefPoly->u * v1 + RefPoly->body->position;
//	v2 = RefPoly->u * v2 + RefPoly->body->position;
//
//	// Calculate reference face side normal in world space
//	sf::Vector2f sidePlaneNormal = (v2 - v1);
//	sidePlaneNormal.Normalize();
//
//	// Orthogonalize
//	sf::Vector2f refFaceNormal(sidePlaneNormal.y, -sidePlaneNormal.x);
//
//	// ax + by = c
//	// c is distance from origin
//	float refC = Dot(refFaceNormal, v1);
//	float negSide = -Dot(sidePlaneNormal, v1);
//	float posSide = Dot(sidePlaneNormal, v2);
//
//	// Clip incident face to reference face side planes
//	if (Clip(-sidePlaneNormal, negSide, incidentFace) < 2)
//		return; // Due to floating point error, possible to not have required points
//
//	if (Clip(sidePlaneNormal, posSide, incidentFace) < 2)
//		return; // Due to floating point error, possible to not have required points
//
//				// Flip
//	manifold->SetNormal(flip ? -refFaceNormal : refFaceNormal);
//
//	// Keep points behind reference face
//	int cp = 0; // clipped points behind reference face
//	float separation = Dot(refFaceNormal, incidentFace[0]) - refC;
//	if (separation <= 0.0f)
//	{
//		manifold->contacts[cp] = incidentFace[0];
//		manifold->penetration = -separation;
//		++cp;
//	}
//	else
//		manifold->penetration = 0;
//
//	separation = Dot(refFaceNormal, incidentFace[1]) - refC;
//	if (separation <= 0.0f)
//	{
//		manifold->contacts[cp] = incidentFace[1];
//
//		manifold->penetration += -separation;
//		++cp;
//
//		// Average penetration
//		manifold->SetPenetration(manifold->GetPenetration() /= (float)cp);
//	}
//
//	m->contact_count = cp;
}
