#pragma once

#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\RenderStates.hpp>
#include <SFML\Graphics\Drawable.hpp>
#include <SFML\Graphics\Transformable.hpp>
#include <SFML\Graphics\VertexArray.hpp>
#include <SFML\System\Time.hpp>

namespace apex
{
	class ParticleEmitter : public sf::Drawable, public sf::Transformable
	{
	public:
		ParticleEmitter(unsigned int count);
		virtual ~ParticleEmitter();

		ParticleEmitter(const ParticleEmitter&) = delete;
		ParticleEmitter& operator=(const ParticleEmitter&) = delete;

		void Tick(sf::Time elapsed);
		void Move(sf::Vector2f newPosition);

	private:
		struct Particle
		{
			sf::Vector2f velocity;
			sf::Time lifetime;
		};

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
		void ResetParticle(size_t index);

		std::vector<Particle> m_Particles;
		sf::VertexArray m_Verticies;
		sf::Time m_Lifetime;
		sf::Vector2f m_CenterPosition;
	};
} // namespace apex
