#pragma once

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

#include <APEX\IO\KeyListener.h>
#include <APEX\IO\MouseListener.h>

namespace apex
{
	class AbstractGame : public apex::KeyListener, public apex::MouseListener
	{
	public:
		AbstractGame();
		virtual ~AbstractGame();

		AbstractGame(const AbstractGame&) = delete;
		AbstractGame& operator=(const AbstractGame&) = delete;

		virtual void Tick(sf::Time elapsed) = 0;
		virtual void Draw(sf::RenderTarget& target) = 0;

		virtual bool OnKeyPress(apex::Keyboard::Key key, bool keyPressed) { return false; };
		virtual bool OnUnmappedKeyPress(int vkCode, bool keyPressed) override { return false; };
		virtual void OnKeyRelease(apex::Keyboard::Key key) override {};

		virtual bool OnButtonPress(sf::Event::MouseButtonEvent buttonEvent) override { return false; };
		virtual void OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent) override {};
		virtual void OnScroll(sf::Event::MouseWheelScrollEvent scrollEvent) override {};

	private:

	};
} // namespace apex
