#pragma once

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

namespace apex
{
	class Particle
	{
	public:
		Particle(sf::Vector2f position, sf::Vector2f velocity = sf::Vector2f());
		virtual ~Particle();

		Particle(const Particle&) = delete;
		Particle& operator=(const Particle&) = delete;

		virtual void Tick(sf::Time elapsed) = 0;
		virtual void Draw(sf::RenderTarget& target, sf::RenderStates states) = 0;

		virtual bool IsAlive() const;

	protected:
		sf::Vector2f m_Pos;
		sf::Vector2f m_Vel;

		bool m_IsAlive = true;

	};
} // namespace apex
