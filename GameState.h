#pragma once

#include "BaseState.h"
#include "ParticleEmitter.h"
#include "Game.h"

class GameState : public BaseState
{
public:
	GameState(StateManager* manager, Game* game);
	virtual ~GameState();

	GameState(const GameState&) = delete;
	GameState& operator=(const GameState&) = delete;

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target) const;
	void TogglePaused();

private:
	sf::ConvexShape CreateStar(sf::Vector2f centerPos, size_t numPoints, float innerRadius, float outerRadius);

	bool m_Paused = false;
	sf::View m_View; // (aka view matrix)
	Game* m_Game = nullptr;
	ParticleEmitter* m_ParticleEmmiter = nullptr;
	sf::ConvexShape star;

};

