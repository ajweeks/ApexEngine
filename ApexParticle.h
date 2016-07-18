#pragma once

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

class ApexParticle
{
public:
	ApexParticle(sf::Vector2f position, sf::Vector2f velocity  = sf::Vector2f());
	virtual ~ApexParticle();

	ApexParticle(const ApexParticle&) = delete;
	ApexParticle& operator=(const ApexParticle&) = delete;

	virtual void Tick(sf::Time elapsed) = 0;
	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states) = 0;

	virtual bool IsAlive() const;

protected:
	sf::Vector2f m_Pos;
	sf::Vector2f m_Vel;

	bool m_IsAlive = true;

};
