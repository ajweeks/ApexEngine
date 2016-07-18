#pragma once

#include "ApexParticle.h"

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

#include <list>

class ApexParticleManager
{
public:
	ApexParticleManager();
	virtual ~ApexParticleManager();

	ApexParticleManager(const ApexParticleManager&) = delete;
	ApexParticleManager& operator=(const ApexParticleManager&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

	void AddParticle(ApexParticle* particle);

private:
	std::list<ApexParticle*> m_Particles;

};
