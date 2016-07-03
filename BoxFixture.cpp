
#include "BoxFixture.h"
#include "PhysicsActor.h"
#include "CircleFixture.h"
#include "ApexMath.h"

BoxFixture::BoxFixture(PhysicsActor* parentActor, sf::Vector2f size) :
	Fixture(parentActor, Shape::BOX),
	m_Size(size)
{
	m_BoundingRectangle.setFillColor(m_FillColor);
	m_BoundingRectangle.setOutlineColor(m_OutlineColor);
	m_BoundingRectangle.setOutlineThickness(m_OutlineThickness);
	m_BoundingRectangle.setSize(size);
	m_RelativePos = sf::Vector2f(-size.x / 2, -size.y / 2);
	m_BoundingRectangle.setPosition(m_ParentActor->GetPosition() + m_RelativePos);

	const float halfWidth = size.x / 2.0f;
	const float halfHeight = size.y / 2.0f;
	m_VertexCount = 4;
	m_Verticies.resize(m_VertexCount);
	m_Normals.resize(m_VertexCount);
	m_Verticies[0] = sf::Vector2f(-halfWidth, -halfHeight);
	m_Verticies[1] = sf::Vector2f(halfWidth, -halfHeight);
	m_Verticies[2] = sf::Vector2f(halfWidth, halfHeight);
	m_Verticies[3] = sf::Vector2f(-halfWidth, halfHeight);
	m_Normals[0] = sf::Vector2f( 0.0f, -1.0f);
	m_Normals[1] = sf::Vector2f( 1.0f,  0.0f);
	m_Normals[2] = sf::Vector2f( 0.0f,  1.0f);
	m_Normals[3] = sf::Vector2f(-1.0f,  0.0f);

	ComputeMass(1.0f);
}

BoxFixture::~BoxFixture()
{
}

void BoxFixture::Tick(sf::Time elapsed)
{
	if (m_ParentActor->GetBodyType() != PhysicsActor::BodyType::STATIC)
	{
		m_BoundingRectangle.setPosition(m_ParentActor->GetPosition() + m_RelativePos);
	}
}

void BoxFixture::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	//if (highlighted) m_BoundingRectangle.setFillColor(m_FillColor + sf::Color(45, 45, 45, 50));
	//else m_BoundingRectangle.setFillColor(m_FillColor);
	target.draw(m_BoundingRectangle, states);
}

sf::Vector2f BoxFixture::GetSize() const
{
	return m_Size;
}

size_t BoxFixture::GetVertexCount() const
{
	return m_VertexCount;
}

std::vector<sf::Vector2f> BoxFixture::GetNormals() const
{
	return m_Normals;
}

std::vector<sf::Vector2f> BoxFixture::GetVerticies() const
{
	return m_Verticies;
}

void BoxFixture::ComputeMass(float density)
{
	sf::Vector2f centroid(0.0f, 0.0f);
	float area = 0.0f;
	float I = 0.0f;
	const float k_inv3 = 1.0f / 3.0f;

	for (size_t i1 = 0; i1 < m_VertexCount; ++i1)
	{
		sf::Vector2f p1(m_Verticies[i1]);
		size_t i2 = i1 + 1 < m_VertexCount ? i1 + 1 : 0;
		sf::Vector2f p2(m_Verticies[i2]);

		float D = ApexMath::Cross(p1, p2);
		float triangleArea = 0.5f * D;

		area += triangleArea;

		centroid += triangleArea * k_inv3 * (p1 + p2);

		float intx2 = p1.x * p1.x + p2.x * p1.x + p2.x * p2.x;
		float inty2 = p1.y * p1.y + p2.y * p1.y + p2.y * p2.y;
		I += (0.25f * k_inv3 * D) * (intx2 + inty2);
	}

	centroid *= 1.0f / area;

	for (size_t i = 0; i < m_VertexCount; i++)
		m_Verticies[i] -= centroid;

	const float mass = (density * area);
	if (mass == 0.0f) m_ParentActor->SetInverseMass(0.0f);
	else m_ParentActor->SetInverseMass(1.0f / mass);
	const float inertia = (density * I);
	if (inertia == 0.0f) m_ParentActor->SetInverseInertia(0.0f);
	else m_ParentActor->SetInverseInertia(1.0f / inertia);
}

bool BoxFixture::IsPointInFixture(sf::Vector2f point) const
{
	const sf::Vector2f ourPos = m_ParentActor->GetPosition();// + m_RelativePos;
	return point.x > ourPos.x &&  point.x < ourPos.x + m_Size.x &&
			point.y > ourPos.y &&  point.y < ourPos.y + m_Size.y;
}