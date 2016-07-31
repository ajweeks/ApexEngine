
#include "GameState.h"
#include "ApexMain.h"
#include "enumerations.h"
#include "ApexKeyboard.h"
#include "ApexAudio.h"
#include "Level.h"

GameState::GameState() :
	BaseState(StateType::GAME)
{
	m_Level = new Level(0);
	APEX->SetPhysicsPaused(false);

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
	m_Level->Tick(elapsed);
}

void GameState::Draw(sf::RenderTarget& target)
{
	m_Level->Draw(target, sf::RenderStates::Default);
}

Level* GameState::GetLevel()
{
	return m_Level;
}

bool GameState::IsLevelPaused() const
{
	return m_Level->IsPaused();
}

bool GameState::OnKeyPress(ApexKeyboard::Key key, bool keyPressed)
{
	if (APEX->DEBUGIsGamePaused()) return true;

	if (keyPressed)
	{
		switch (key)
		{
		case ApexKeyboard::PAUSE:
		{
			m_Level->TogglePaused(true);
		} break;
		case ApexKeyboard::DEBUG_TOGGLE_DEBUG_OVERLAY:
		{
				m_Level->ToggleDebugOverlay();
		} break;
		case ApexKeyboard::DEBUG_TOGGLE_LIGHT_EDITOR:
		{
			m_Level->ToggleLightingEditor();
		} break;
		case ApexKeyboard::DEBUG_RESTART:
		{
			if (!m_Level->IsPaused()) Reset();
		} break;
		}
	}
	return false;
}

void GameState::OnKeyRelease(ApexKeyboard::Key key)
{
	if (APEX->DEBUGIsGamePaused()) return;
}

//static sf::ConvexShape CreateStar(sf::Vector2f centerPos, size_t numPoints, float innerRadius, float outerRadius)
//{
//	if (numPoints == 0) return sf::ConvexShape();
//
//	sf::ConvexShape star(numPoints * 2);
//	star.setPosition(centerPos);
//	star.setOrigin(centerPos);
//
//	const float deltaAngle = float((2.0f * 3.14159f) / (numPoints * 2));
//	float currentAngle = 0.0f;
//	for (size_t i = 0; i < numPoints * 2; ++i)
//	{
//		const float radius = (i % 2 == 0 ? outerRadius : innerRadius);
//		star.setPoint(i, sf::Vector2f(centerPos.x + cos(currentAngle) * radius, centerPos.y + sin(currentAngle) * radius));
//		currentAngle += deltaAngle;
//	}
//
//	return star;
//}
