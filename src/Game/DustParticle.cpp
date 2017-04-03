
#include "Game\DustParticle.h"
#include "Game\enumerations.h"

#include "Apex\Graphics\SpriteSheet.h"
#include "Apex\Graphics\TextureManager.h"

DustParticle::DustParticle(sf::Vector2f position) :
	apex::Particle(position, sf::Vector2f())
{
	m_SpriteSheet = new apex::SpriteSheet(apex::TextureManager::GetTexture(Texture::DUST_PARTICLE), 36, 36);
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
