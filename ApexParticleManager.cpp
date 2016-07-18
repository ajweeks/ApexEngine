
#include "ApexParticleManager.h"

typedef std::list<ApexParticle*>::iterator ParticleIterator;

ApexParticleManager::ApexParticleManager()
{
}

ApexParticleManager::~ApexParticleManager()
{
	ParticleIterator iter = m_Particles.begin();
	while (iter != m_Particles.end())
	{
		delete *iter;
		++iter;
	}
	m_Particles.clear();
}

void ApexParticleManager::Tick(sf::Time elapsed)
{
	std::list<ApexParticle*>::iterator iter = m_Particles.begin();
	while (iter != m_Particles.end())
	{
		(*iter)->Tick(elapsed);

		if (!(*iter)->IsAlive())
		{
			iter = m_Particles.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void ApexParticleManager::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	ParticleIterator iter = m_Particles.begin();
	while (iter != m_Particles.end())
	{
		(*iter)->Draw(target, states);
		++iter;
	}
}

void ApexParticleManager::AddParticle(ApexParticle* particle)
{
	m_Particles.push_back(particle);
}
