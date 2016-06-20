
#include "GameState.h"
#include "enumerations.h"
#include "ApexKeyboard.h"
#include "ApexAudio.h"
#include "Level.h"

#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

GameState::GameState(StateManager* manager, Game* game)
	: BaseState(manager, StateType::GAME, game)
{
	m_Level = new Level(game);

	Reset();
}

GameState::~GameState()
{
	delete m_Level;
}

void GameState::Reset()
{
	m_Level->Reset();
}

void GameState::Tick(sf::Time elapsed)
{
	if (ApexKeyboard::IsKeyPressed(sf::Keyboard::Space))
		TogglePaused(true);
	if (ApexKeyboard::IsKeyPressed(sf::Keyboard::R))
		Reset();

	if (ApexKeyboard::IsKeyPressed(sf::Keyboard::S))
		ApexAudio::PlaySoundEffect(ApexAudio::Sound::COIN);
	if (ApexKeyboard::IsKeyPressed(sf::Keyboard::F))
		ApexAudio::PlaySoundEffect(ApexAudio::Sound::WALK_WOOD);
	if (ApexKeyboard::IsKeyPressed(sf::Keyboard::M))
		ApexAudio::PlayMusicTrack(ApexAudio::Music::BG_SONG);
	if (ApexKeyboard::IsKeyPressed(sf::Keyboard::L))
		ApexAudio::PlayMusicTrack(ApexAudio::Music::OVERWORLD_BGM);

	if (m_Paused) return;

	m_Level->Tick(elapsed);
}

void GameState::Draw(sf::RenderTarget& target) const
{
	m_Level->Draw(target, sf::RenderStates::Default);
}

void GameState::TogglePaused(bool pauseSounds)
{
	m_Paused = !m_Paused;
	if (pauseSounds) ApexAudio::SetAllPaused(m_Paused);
}

Level* GameState::GetLevel()
{
	return m_Level;
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
