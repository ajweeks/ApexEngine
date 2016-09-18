#pragma once

#include <APEX\Transition\Transition.h>

#include <SFML\System\Vector2.hpp>
#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\Transformable.hpp>

namespace apex
{

	class IntTransition : public Transition
	{
	public:
		IntTransition();
		IntTransition(int start, int end, sf::Time totalTime, EaseType easeType = EaseType::LINEAR);
		virtual ~IntTransition();

		void Create(int start, int end, sf::Time totalTime, EaseType easeType = EaseType::LINEAR);

		virtual void Tick(sf::Time elapsed) override;
		virtual int GetCurrentTransitionData();

		virtual void SwapAndRestart() override; // Swap start and end data, and reset

	private:
		int m_StartValue;
		int m_EndValue;

	};
} // namespace apex