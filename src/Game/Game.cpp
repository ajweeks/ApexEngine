
#include "Game\Game.h"
#include "Game\World\LightManager.h"
#include "Game\World\Map.h"
#include "Game\State\LoadingState.h"
#include "Game\Debug\ImGui.h"

#include "Apex\ApexMain.h"
#include "Apex\Transition\ColorTransition.h"
#include "Apex\IO\Audio.h"

sf::Font Game::FontOpenSans;
sf::Font Game::FontPixelFJ8;

bool Game::s_ShowingDebugOverlay = false;

Game::Game()
{
	m_SlowMoData.Create(0.01f, 1.0f, sf::seconds(0.1f), apex::Transition::EaseType::QUADRATIC_IN_OUT);
	m_SlowMoData.SetFinished();

	APEX->SetWindowTitle("Crowchild");

	LoadingState* loadingState = new LoadingState(m_StateManager);
	m_StateManager.SetState(loadingState);

	ImGui::Init();
}

Game::~Game()
{
}

void Game::Tick(sf::Time elapsed)
{
	m_StateManager.Tick(elapsed);
}

void Game::Draw(sf::RenderTarget& target)
{
	m_StateManager.Draw(target);

	if (s_ShowingDebugOverlay)
	{
		ImGui::Draw(target);
	}
}

void Game::SetSlowMoTime(sf::Time duration, apex::Transition::EaseType easeType)
{
	m_SlowMoData.SetDuration(duration);
	m_SlowMoData.SetEaseType(easeType);
	m_SlowMoData.Restart();
}

bool Game::OnKeyPress(apex::Keyboard::Key key, bool keyPressed)
{
	if (s_ShowingDebugOverlay)
	{
		if (ImGui::OnKeyPress(key, keyPressed)) return false;
	}

	switch (Key(key.vkCode))
	{
	case Key::SCREENSHOT:
	{
		APEX->TakeScreenshot();
	} break;
	case Key::DEBUG_TOGGLE_INFO_OVERLAY:
	{
		ToggleDebugOverlay();
	} break;
	case Key::DEBUG_PAUSE_EVERYTHING:
	{
		APEX->ToggleGamePaused();
	} break;
	case Key::DEBUG_STEP_ONE_PHYSICS_FRAME:
	{
		if (APEX->IsGamePaused()) APEX->SetStepOneFrame(true);
	} break;
	case Key::DEBUG_TOGGLE_PHYSICS_OVERLAY:
	{
		APEX->ToggleShowingPhysicsDebug();
	} break;
	}
	return false;
}

void Game::OnKeyRelease(apex::Keyboard::Key key)
{
	if (s_ShowingDebugOverlay)
	{
		ImGui::OnKeyRelease(key);
	}
}

bool Game::OnButtonPress(sf::Event::MouseButtonEvent buttonEvent)
{
	if (s_ShowingDebugOverlay)
	{
		if (ImGui::OnButtonPress(buttonEvent)) return false;
	}
	return false;
}

void Game::OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent)
{
	if (s_ShowingDebugOverlay)
	{
		ImGui::OnButtonRelease(buttonEvent);
	}
}

void Game::ToggleDebugOverlay()
{
	s_ShowingDebugOverlay = !s_ShowingDebugOverlay;
}

bool Game::IsShowingDebugOverlay()
{
	return s_ShowingDebugOverlay;
}

std::string Game::GetKeyName(size_t keyIndex)
{
	switch (Key(keyIndex))
	{
	case Key::MOVE_UP:
		return "Move Up";
	case Key::MOVE_LEFT:
		return "Move Left";
	case Key::MOVE_DOWN:
		return "Move Down";
	case Key::MOVE_RIGHT:
		return "Move Right";
	case Key::PAUSE:
		return "Pause";
	case Key::INTERACT:
		return "Interact";
	case Key::SCREENSHOT:
		return "Screenshot";
	case Key::DEBUG_RESTART:
		return "DEBUG_Restart";
	case Key::DEBUG_TOGGLE_INFO_OVERLAY:
		return "DEBUG_ToggleInfoOverlay";
	case Key::DEBUG_TOGGLE_LIGHT_EDITOR:
		return "DEBUG_ToggleLightEditor";
	case Key::DEBUG_TOGGLE_PHYSICS_OVERLAY:
		return "DEBUG_TogglePhysicsOverlay";
	case Key::DEBUG_STEP_ONE_PHYSICS_FRAME:
		return "DEBUG_StepOnePhysicsFrame";
	case Key::DEBUG_PAUSE_EVERYTHING:
		return "DEBUG_PauseEverything";
	case Key::NONE:
	default:
		return "Unknown";
	}
}
