#pragma once

#include "Apex\Transition\Transition.h"

#include <SFML\System\Vector2.hpp>
#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\Transformable.hpp>

namespace apex
{
	class ColorTransition : public Transition
	{
	public:
		ColorTransition();
		ColorTransition(sf::Color start, sf::Color end, sf::Time totalTime, EaseType easeType = EaseType::LINEAR);
		virtual ~ColorTransition();

		void Create(sf::Color start, sf::Color end, sf::Time totalTime, EaseType easeType = EaseType::LINEAR);

		virtual void Tick(sf::Time elapsed) override;
		virtual sf::Color GetCurrentColor();

		virtual void SwapAndRestart() override; // Swap start and end data, and reset

	private:
		sf::Color m_StartColor;
		sf::Color m_EndColor;

	};
} // namespace apex
