
#include "ApexParticle.h"

ApexParticle::ApexParticle(sf::Vector2f position, sf::Vector2f velocity) :
	m_Pos(position), m_Vel(velocity)
{
}

ApexParticle::~ApexParticle()
{
}

bool ApexParticle::IsAlive() const
{
	return m_IsAlive;
}
