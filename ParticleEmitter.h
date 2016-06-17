#pragma once

#include <SFML\Graphics.hpp>

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
