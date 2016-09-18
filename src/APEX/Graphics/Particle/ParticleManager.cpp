
#include <APEX\Graphics\Particle\ParticleManager.h>

namespace apex
{
	typedef std::list<Particle*>::iterator ParticleIterator;
	
	ParticleManager::ParticleManager()
	{
	}

	ParticleManager::~ParticleManager()
	{
		for (size_t i = 0; i < m_Particles.size(); ++i)
		{
			delete (*m_Particles.begin() + i);
		}
		m_Particles.clear();
	}

	void ParticleManager::Tick(sf::Time elapsed)
	{
		std::list<Particle*>::iterator iter = m_Particles.begin();
		while (iter != m_Particles.end())
		{
			(*iter)->Tick(elapsed);

			if (!(*iter)->IsAlive())
			{
				delete (*iter);
				iter = m_Particles.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}

	void ParticleManager::Draw(sf::RenderTarget& target, sf::RenderStates states)
	{
		ParticleIterator iter = m_Particles.begin();
		while (iter != m_Particles.end())
		{
			(*iter)->Draw(target, states);
			++iter;
		}
	}

	void ParticleManager::AddParticle(Particle* particle)
	{
		m_Particles.push_back(particle);
	}
} // namespace apex
