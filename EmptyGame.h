#pragma once

#include <APEX\AbstractGame.h>

class EmptyGame : public apex::AbstractGame
{
public:
	EmptyGame();
	virtual ~EmptyGame();

	EmptyGame(const EmptyGame&) = delete;
	EmptyGame& operator=(const EmptyGame&) = delete;

	virtual void Tick(sf::Time elapsed);
	virtual void Draw(sf::RenderTarget& target);

private:

};
