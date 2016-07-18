#pragma once

#include "ApexParticle.h"

class ApexSpriteSheet;

class DustParticle : public ApexParticle
{
public:
	DustParticle(sf::Vector2f position);
	virtual ~DustParticle();

	DustParticle(const DustParticle&) = delete;
	DustParticle& operator=(const DustParticle&) = delete;

	virtual void Tick(sf::Time elapsed);
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states);

private:
	static ApexSpriteSheet* m_SpriteSheet;
	static int m_InstaceCount;

};
