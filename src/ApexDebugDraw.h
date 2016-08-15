#pragma once

#include <Box2D\Common\b2Draw.h>

#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\ConvexShape.hpp>
#include <SFML\Graphics\CircleShape.hpp>

struct b2AABB;

class ApexDebugDraw : public b2Draw
{
public:
	ApexDebugDraw(sf::RenderTarget& target);
	virtual ~ApexDebugDraw();

	void Create();
	void Destroy();

	static sf::Vector2f b2Vec2ToSFVec2f(b2Vec2 vec);
	static sf::Color b2ColorToSFColor(b2Color col);
	static sf::Color b2ColorToSFColor(b2Color col, float percentage);

	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) override;
	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) override;
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;
	void DrawTransform(const b2Transform& xf) override;
	void DrawString(int x, int y, const char* string, ...);
	void DrawString(const b2Vec2& p, const char* string, ...);
	void DrawAABB(b2AABB* aabb, const b2Color& color);

private:
	static const sf::Color m_SFRed;
	static const sf::Color m_SFGreen;

	sf::RenderTarget& m_Target;
	sf::ConvexShape m_Polygon;
	sf::CircleShape m_Circle;


};