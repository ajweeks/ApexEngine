
#include "ApexDebugDraw.h"
#include "ApexMain.h"
#include "PhysicsActor.h"

#include <SFML\Graphics\VertexArray.hpp>
#include <SFML\Graphics\RectangleShape.hpp>
#include <SFML\Graphics\Text.hpp>
#include <Box2D\Collision\b2Collision.h>

#define BUFFER_OFFSET(x)  ((const void*) (x))

const sf::Color ApexDebugDraw::m_SFRed = sf::Color(255, 0, 0);
const sf::Color ApexDebugDraw::m_SFGreen = sf::Color(0, 255, 0);

ApexDebugDraw::ApexDebugDraw(sf::RenderTarget& target) :
	m_Target(target)
{
	m_Polygon.setOutlineThickness(-0.5f);
	m_Circle.setOutlineThickness(-0.5f);
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

inline sf::Vector2f ApexDebugDraw::b2Vec2ToSFVec2f(b2Vec2 vec)
{
	return sf::Vector2f(vec.x * PhysicsActor::SCALE, vec.y * PhysicsActor::SCALE);
}

inline sf::Color ApexDebugDraw::b2ColorToSFColor(b2Color col)
{
	return sf::Color(static_cast<sf::Uint8>(col.r * 255),
		static_cast<sf::Uint8>(col.g * 255),
		static_cast<sf::Uint8>(col.b * 255),
		static_cast<sf::Uint8>(col.a * 255));
}

inline sf::Color ApexDebugDraw::b2ColorToSFColor(b2Color col, float percentage)
{
	return sf::Color(static_cast<sf::Uint8>(col.r * 255 * percentage),
		static_cast<sf::Uint8>(col.g * 255 * percentage),
		static_cast<sf::Uint8>(col.b * 255 * percentage),
		static_cast<sf::Uint8>(col.a * 255 * percentage));
}

void ApexDebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	m_Polygon.setPointCount(vertexCount);
	for (int32 i = 0; i < vertexCount; ++i)
	{
		m_Polygon.setPoint(i, b2Vec2ToSFVec2f(vertices[i]));
	}
	m_Polygon.setFillColor(sf::Color::Transparent);
	m_Polygon.setOutlineColor(b2ColorToSFColor(color));

	m_Target.draw(m_Polygon);
}

void ApexDebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	m_Polygon.setPointCount(vertexCount);
	for (int32 i = 0; i < vertexCount; ++i)
	{
		m_Polygon.setPoint(i, b2Vec2ToSFVec2f(vertices[i]));
	}
	m_Polygon.setFillColor(b2ColorToSFColor(color, 0.5f));
	m_Polygon.setOutlineColor(b2ColorToSFColor(color));

	m_Target.draw(m_Polygon);
}

void ApexDebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	const float scaledRadius = radius * PhysicsActor::SCALE;
	m_Circle.setRadius(scaledRadius);
	m_Circle.setOrigin(scaledRadius, scaledRadius);
	m_Circle.setPosition(b2Vec2ToSFVec2f(center));
	m_Circle.setOutlineColor(b2ColorToSFColor(color));
	m_Circle.setFillColor(sf::Color::Transparent);

	m_Target.draw(m_Circle);
}

void ApexDebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	const float scaledRadius = radius * PhysicsActor::SCALE;
	m_Circle.setRadius(scaledRadius);
	m_Circle.setOrigin(scaledRadius, scaledRadius);
	m_Circle.setPosition(b2Vec2ToSFVec2f(center));
	m_Circle.setOutlineColor(b2ColorToSFColor(color));
	m_Circle.setFillColor(b2ColorToSFColor(color, 0.5f));

	m_Target.draw(m_Circle);
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
	static const float lineLength = 0.05f;

	b2Vec2 xAxis = b2Vec2(xf.p + lineLength * xf.q.GetXAxis());
	sf::Vertex redLine[2] = {
		sf::Vertex(b2Vec2ToSFVec2f(xf.p), m_SFRed),
		sf::Vertex(b2Vec2ToSFVec2f(xAxis), m_SFRed)
	};

	b2Vec2 yAxis = b2Vec2(xf.p + lineLength * xf.q.GetYAxis());
	sf::Vertex greenLine[2] = {
		sf::Vertex(b2Vec2ToSFVec2f(xf.p), m_SFGreen),
		sf::Vertex(b2Vec2ToSFVec2f(yAxis), m_SFGreen)
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