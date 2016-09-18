
#include <APEX\Graphics\Particle\Particle.h>

namespace apex
{
	Particle::Particle(sf::Vector2f position, sf::Vector2f velocity) :
		m_Pos(position), m_Vel(velocity)
	{
	}

	Particle::~Particle()
	{
	}

	bool Particle::IsAlive() const
	{
		return m_IsAlive;
	}
} // namespace apex
