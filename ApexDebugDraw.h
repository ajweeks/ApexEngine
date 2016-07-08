#pragma once

#include <Box2D\Common\b2Draw.h>

#include <SFML\Graphics\RenderTarget.hpp>

struct b2AABB;

class ApexDebugDraw : public b2Draw
{
public:
	ApexDebugDraw(sf::RenderTarget& target);
	virtual ~ApexDebugDraw();

	void Create();
	void Destroy();

	static sf::Vector2f b2Vec2ToSFVec2f(b2Vec2 vec);
	static sf::Color b2ColorToSFColor(b2Color col, float percentage = 1.0f);
	
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
	sf::RenderTarget& m_Target;

};
