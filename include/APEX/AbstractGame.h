#pragma once

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

namespace apex
{
	class AbstractGame
	{
	public:
		AbstractGame();
		virtual ~AbstractGame();

		AbstractGame(const AbstractGame&) = delete;
		AbstractGame& operator=(const AbstractGame&) = delete;

		virtual void Tick(sf::Time elapsed) = 0;
		virtual void Draw(sf::RenderTarget& target) = 0;

	private:

	};
} // namespace apex
