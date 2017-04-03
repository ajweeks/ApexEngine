
#include "Game\State\GameState.h"
#include "Game\Game.h"
#include "Game\World\World.h"
#include "Game\World\Map.h"
#include "Game\enumerations.h"

#include "Apex\ApexMain.h"
#include "Apex\IO\Keyboard.h"
#include "Apex\IO\Audio.h"

GameState::GameState(apex::StateManager& stateManager) :
	BaseState(int(StateType::GAME), stateManager)
{
	m_World = new World(0, stateManager);
	APEX->SetPhysicsPaused(false);

	Reset();
	APEX->StartFadeOut();
}

GameState::~GameState()
{
	delete m_World;
}

void GameState::Reset()
{
	m_World->Reset();
}

void GameState::Tick(sf::Time elapsed)
{
	m_World->Tick(elapsed);
}

void GameState::Draw(sf::RenderTarget& target)
{
	m_World->Draw(target, sf::RenderStates::Default);
}

World& GameState::GetWorld()
{
	return *m_World;
}

bool GameState::IsWorldPaused() const
{
	return m_World->IsPaused();
}

bool GameState::OnKeyPress(apex::Keyboard::Key key, bool keyPressed)
{
	if (APEX->IsGamePaused()) return false;

	if (keyPressed)
	{
		switch (Key(key.vkCode))
		{
		case Key::DEBUG_TOGGLE_LIGHT_EDITOR:
		{
			m_World->GetCurrentMap()->ToggleLightingEditor();
		} break;
		case Key::DEBUG_RESTART:
		{
			if (!m_World->IsPaused()) Reset();
		} break;
		}
	}
	return false;
}

void GameState::OnKeyRelease(apex::Keyboard::Key key)
{
	if (APEX->IsGamePaused()) return;
}

void GameState::OnUnmappedKeypress(sf::Event::KeyEvent event)
{
	m_World->OnUnmappedKeypress(event);
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
