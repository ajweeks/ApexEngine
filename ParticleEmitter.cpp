#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter(unsigned int count) :
	m_Particles(count),
	m_Verticies(sf::Points, count),
	m_Lifetime(sf::milliseconds(4000)),
	m_CenterPosition(0, 0)
{
}

ParticleEmitter::~ParticleEmitter()
{
}

void ParticleEmitter::Tick(sf::Time elapsed)
{
	for (std::size_t i = 0; i < m_Particles.size(); ++i)
	{
		Particle& p = m_Particles[i];
		p.lifetime -= elapsed;

		if (p.lifetime <= sf::Time::Zero)
		{
			ResetParticle(i);
		}

		m_Verticies[i].position += p.velocity * elapsed.asSeconds();
		const float ratio = p.lifetime.asSeconds() / m_Lifetime.asSeconds();
		m_Verticies[i].color.a = static_cast<sf::Uint8>(ratio * 255);
	}
}

void ParticleEmitter::Move(sf::Vector2f newPosition)
{
	m_CenterPosition = newPosition;
}

void ParticleEmitter::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();

	states.texture = NULL;

	target.draw(m_Verticies);
}

void ParticleEmitter::ResetParticle(size_t index)
{
	m_Particles[index].lifetime = sf::milliseconds(std::rand() % 2000) + sf::milliseconds(2000);
	const float angle = float(std::rand() % 720) * 3.1415f / 360.0f;
	const float speed = float(std::rand() % 600);
	m_Particles[index].velocity = sf::Vector2f(cos(angle) * speed, sin(angle) * speed);

	m_Verticies[index].position = m_CenterPosition;
	m_Verticies[index].color = sf::Color(
		sf::Uint8((std::rand()%255) * 0.8), 
		sf::Uint8((std::rand()%255) * 0.5), 
		sf::Uint8((std::rand()%255) * 0.5));
}
