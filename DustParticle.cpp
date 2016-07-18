
#include "DustParticle.h"
#include "ApexSpriteSheet.h"

ApexSpriteSheet* DustParticle::m_SpriteSheet = nullptr;
int DustParticle::m_InstaceCount = 0;

DustParticle::DustParticle(sf::Vector2f position) :
	ApexParticle(position, sf::Vector2f())
{
	++m_InstaceCount;
	if (m_SpriteSheet == nullptr)
	{
		m_SpriteSheet = new ApexSpriteSheet("resources/dust-explosion.png", 36, 36);
		m_SpriteSheet->SetEntireSpriteAsOneSequence(85);
	}
}

DustParticle::~DustParticle()
{
	if (--m_InstaceCount == 0)
	{
		delete m_SpriteSheet;
		m_SpriteSheet = nullptr;
	}
}

void DustParticle::Tick(sf::Time elapsed)
{
	m_SpriteSheet->Tick(elapsed);
	m_IsAlive = !m_SpriteSheet->HasRestarted();
}

void DustParticle::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	states.transform.translate(m_Pos);
	m_SpriteSheet->Draw(target, states);
}
