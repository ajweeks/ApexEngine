
#include "ApexDebugDraw.h"
#include "ApexMain.h"
#include "PhysicsActor.h"

#include <SFML\Graphics\VertexArray.hpp>
#include <SFML\Graphics\ConvexShape.hpp>
#include <SFML\Graphics\CircleShape.hpp>
#include <SFML\Graphics\RectangleShape.hpp>
#include <SFML\Graphics\Text.hpp>
#include <Box2D\Collision\b2Collision.h>

#define BUFFER_OFFSET(x)  ((const void*) (x))

ApexDebugDraw::ApexDebugDraw(sf::RenderTarget& target) :
	m_Target(target)
{
}

ApexDebugDraw::~ApexDebugDraw()
{
}

void ApexDebugDraw::Create()
{
}

void ApexDebugDraw::Destroy()
{
}

sf::Vector2f ApexDebugDraw::b2Vec2ToSFVec2f(b2Vec2 vec)
{
	return sf::Vector2f(vec.x * PhysicsActor::SCALE, vec.y * PhysicsActor::SCALE);
}

sf::Color ApexDebugDraw::b2ColorToSFColor(b2Color col, float percentage)
{
	return sf::Color(static_cast<sf::Uint8>(col.r * 255 * percentage), 
					static_cast<sf::Uint8>(col.g * 255 * percentage),
					static_cast<sf::Uint8>(col.b * 255 * percentage),
					static_cast<sf::Uint8>(col.a * 255 * percentage));
}

void ApexDebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	sf::ConvexShape polygon(vertexCount);
	for (int32 i = 0; i < vertexCount; ++i)
	{
		polygon.setPoint(i, b2Vec2ToSFVec2f(vertices[i]));
	}
	polygon.setOutlineThickness(-1.0f);
	polygon.setFillColor(sf::Color::Transparent);
	polygon.setOutlineColor(b2ColorToSFColor(color));

	m_Target.draw(polygon);
}

void ApexDebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	sf::ConvexShape polygon(vertexCount);
	for (int32 i = 0; i < vertexCount; ++i)
	{
		polygon.setPoint(i, b2Vec2ToSFVec2f(vertices[i]));
	}
	polygon.setOutlineThickness(-1.0f);
	polygon.setFillColor(b2ColorToSFColor(color, 0.5f));
	polygon.setOutlineColor(b2ColorToSFColor(color));

	m_Target.draw(polygon);
}

void ApexDebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	sf::CircleShape circle(radius * PhysicsActor::SCALE);
	circle.setOrigin(radius * PhysicsActor::SCALE, radius * PhysicsActor::SCALE);
	circle.setPosition(b2Vec2ToSFVec2f(center));
	circle.setOutlineThickness(-1.0f);
	circle.setOutlineColor(b2ColorToSFColor(color));
	circle.setFillColor(sf::Color::Transparent);

	m_Target.draw(circle);
}

void ApexDebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	sf::CircleShape circle(radius * PhysicsActor::SCALE);
	circle.setOrigin(radius * PhysicsActor::SCALE, radius * PhysicsActor::SCALE);
	circle.setPosition(b2Vec2ToSFVec2f(center));
	circle.setOutlineThickness(-1.0f);
	circle.setOutlineColor(b2ColorToSFColor(color));
	circle.setFillColor(b2ColorToSFColor(color, 0.5f));

	b2Vec2 endPoint = center + radius * axis;
	sf::Vertex line[2] =
	{
		sf::Vertex(b2Vec2ToSFVec2f(center), b2ColorToSFColor(color)),
		sf::Vertex(b2Vec2ToSFVec2f(endPoint), b2ColorToSFColor(color)),
	};

	m_Target.draw(circle);
	m_Target.draw(line, 2, sf::Lines);
}

void ApexDebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	const sf::Color lineColor(b2ColorToSFColor(color));

	sf::Vertex segment[2];
	segment[0] = sf::Vertex(b2Vec2ToSFVec2f(p1), lineColor);
	segment[1] = sf::Vertex(b2Vec2ToSFVec2f(p2), lineColor);

	m_Target.draw(segment, 2, sf::Lines);
}

void ApexDebugDraw::DrawTransform(const b2Transform& xf)
{
	sf::Color red(255, 0, 0);
	sf::Color green(0, 255, 00);

	const float lineLength = 0.2f;

	b2Vec2 xAxis = b2Vec2(xf.p + lineLength * xf.q.GetXAxis());
	sf::Vertex redLine[2] = {
		sf::Vertex(b2Vec2ToSFVec2f(xf.p), red),
		sf::Vertex(b2Vec2ToSFVec2f(xAxis), red)
	};

	b2Vec2 yAxis = b2Vec2(xf.p + lineLength * xf.q.GetYAxis());
	sf::Vertex greenLine[2] = {
		sf::Vertex(b2Vec2ToSFVec2f(xf.p), green),
		sf::Vertex(b2Vec2ToSFVec2f(yAxis), green)
	};

	m_Target.draw(redLine, 2, sf::Lines);
	m_Target.draw(greenLine, 2, sf::Lines);
}

void ApexDebugDraw::DrawString(int x, int y, const char *string, ...)
{
	sf::Text text(sf::String(string), APEX->FontOpenSans);
	text.setPosition(float(x), float(y));
	m_Target.draw(text);
}

void ApexDebugDraw::DrawString(const b2Vec2& pw, const char *string, ...)
{
	sf::Text text(sf::String(string), APEX->FontOpenSans);
	text.setPosition(pw.x, pw.y);
	m_Target.draw(text);
}

void ApexDebugDraw::DrawAABB(b2AABB* aabb, const b2Color& c)
{
	sf::RectangleShape rectangle(sf::Vector2f(aabb->GetExtents().x, aabb->GetExtents().y));
	rectangle.setPosition(aabb->GetCenter().x, aabb->GetCenter().y);
	rectangle.setFillColor(b2ColorToSFColor(c));
	m_Target.draw(rectangle);
}
