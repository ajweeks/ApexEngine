#pragma once

#include "Apex\Graphics\Particle\Particle.h"

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

#include <list>

namespace apex
{
	class ParticleManager
	{
	public:
		ParticleManager();
		virtual ~ParticleManager();

		ParticleManager(const ParticleManager&) = delete;
		ParticleManager& operator=(const ParticleManager&) = delete;

		void Tick(sf::Time elapsed);
		void Draw(sf::RenderTarget& target, sf::RenderStates states);

		void AddParticle(Particle* particle);

	private:
		std::list<Particle*> m_Particles;

	};
} // namespace apex
