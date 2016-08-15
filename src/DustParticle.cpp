
#include "DustParticle.h"
#include "ApexSpriteSheet.h"
#include "TextureManager.h"

DustParticle::DustParticle(sf::Vector2f position) :
	ApexParticle(position, sf::Vector2f())
{
	m_SpriteSheet = new ApexSpriteSheet(TextureManager::GetTexture(TextureManager::DUST_PARTICLE), 36, 36);
	m_SpriteSheet->SetEntireSpriteAsOneSequence(85);
}

DustParticle::~DustParticle()
{
	delete m_SpriteSheet;
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
