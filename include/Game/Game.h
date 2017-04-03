#pragma once

#include "enumerations.h"

#include "Apex\AbstractGame.h"
#include "Apex\Transition\Transition.h"
#include "Apex\Transition\FloatTransition.h"
#include "Apex\Transition\TransitionChain.h"
#include "Apex\State\StateManager.h"

#include <SFML\System\Time.hpp>

#include <memory>

class Game : public apex::AbstractGame
{
public:
	Game();
	virtual ~Game();

	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;

	void Tick(sf::Time elapsed) override;
	void Draw(sf::RenderTarget& target) override;

	void SetSlowMoTime(sf::Time duration, apex::Transition::EaseType easeType); // Call this with the amount of time to be in slow mo for

	static sf::Font FontOpenSans;
	static sf::Font FontPixelFJ8;
	
	static std::string GetKeyName(size_t keyIndex);

	static void ToggleDebugOverlay();
	static bool IsShowingDebugOverlay();

	virtual bool OnKeyPress(apex::Keyboard::Key key, bool keyPressed) override;
	virtual void OnKeyRelease(apex::Keyboard::Key key) override;

	virtual bool OnButtonPress(sf::Event::MouseButtonEvent buttonEvent) override;
	virtual void OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent) override;

private:
	static bool s_ShowingDebugOverlay;

	apex::StateManager m_StateManager;
	apex::FloatTransition m_SlowMoData;

};