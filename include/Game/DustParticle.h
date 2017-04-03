#pragma once

#include "Apex\Graphics\Particle\Particle.h"

namespace apex
{
	class SpriteSheet;
}

class DustParticle : public apex::Particle
{
public:
	DustParticle(sf::Vector2f position);
	virtual ~DustParticle();

	DustParticle(const DustParticle&) = delete;
	DustParticle& operator=(const DustParticle&) = delete;

	virtual void Tick(sf::Time elapsed);
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states);

private:
	apex::SpriteSheet* m_SpriteSheet;
};
