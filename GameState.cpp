
#include "GameState.h"
#include "enumerations.h"
#include "ApexKeyboard.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

GameState::GameState(StateManager* manager, Game* game)
	: BaseState(manager, StateType::GAME), 
	m_Game(game)
{
	m_View.reset(sf::FloatRect(0, 0, 1920, 1080));

	m_ParticleEmmiter = new ParticleEmitter(20000);

	star = CreateStar(sf::Vector2f(300.0f, 450.0f), 3, 150.0f, 300.0f);
	star.setFillColor(sf::Color(100, 130, 210));
}

GameState::~GameState()
{
	delete m_ParticleEmmiter;
}

void GameState::Tick(sf::Time elapsed)
{
	if (ApexKeyboard::IsKeyPressed(sf::Keyboard::Space))
	{
		TogglePaused();
	}
	if (m_Paused) return;

	m_ParticleEmmiter->Move(m_Game->GetMouseCoordsWorldSpace());

	m_ParticleEmmiter->Tick(elapsed);
	if (ApexKeyboard::IsKeyDown(sf::Keyboard::D) || ApexKeyboard::IsKeyDown(sf::Keyboard::Right))
		star.rotate(1);
	else if (ApexKeyboard::IsKeyDown(sf::Keyboard::A) || ApexKeyboard::IsKeyDown(sf::Keyboard::Left))
		star.rotate(-1);
}

void GameState::Draw(sf::RenderTarget& target) const
{
	target.draw(*m_ParticleEmmiter);
	target.draw(star);
}

void GameState::TogglePaused()
{
	m_Paused = !m_Paused;
}

sf::ConvexShape GameState::CreateStar(sf::Vector2f centerPos, size_t numPoints, float innerRadius, float outerRadius)
{
	if (numPoints == 0) return sf::ConvexShape();

	sf::ConvexShape star(numPoints * 2);
	star.setPosition(centerPos);
	star.setOrigin(centerPos);

	const float deltaAngle = float((2 * M_PI) / (numPoints * 2));
	float currentAngle = 0.0f;
	for (size_t i = 0; i < numPoints * 2; ++i)
	{
		const float radius = (i % 2 == 0 ? outerRadius : innerRadius);
		star.setPoint(i, sf::Vector2f(centerPos.x + cos(currentAngle) * radius, centerPos.y + sin(currentAngle) * radius));
		currentAngle += deltaAngle;
	}

	return star;
}
