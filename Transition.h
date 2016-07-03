#pragma once

#include <SFML\System\Vector2.hpp>
#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\RenderStates.hpp>
#include <SFML\Graphics\Transformable.hpp>

struct TransitionData
{
	sf::Transformable transformable;
	sf::Color color;
};

class Transition
{
public:
	// TODO: Add custom curve editor?!?!
	enum class EaseType
	{
		LINEAR,
		QUADRATIC_IN,
		QUADRATIC_OUT,
		QUADRATIC_IN_OUT,
		CIRCULAR_IN,
		CIRCULAR_OUT,
		CIRCULAR_IN_OUT,
		EXPONENTIAL_IN,
		EXPONENTIAL_OUT,
		EXPONENTIAL_IN_OUT
	};

	Transition();
	Transition(TransitionData start, TransitionData end, sf::Time totalTime, EaseType easeType);
	virtual ~Transition();

	void Create(TransitionData start, TransitionData end, sf::Time totalTime, EaseType easeType);

	void Tick(sf::Time elapsed);
	TransitionData GetCurrentTransitionData();

	void Restart();
	void Swap(); // Swap start and end data, and reset
	void SetEaseType(EaseType easeType);

private:
	sf::Vector2f Lerp(sf::Vector2f start, sf::Vector2f delta, float t);
	float Lerp(float start, float delta, float t); // t must be in range [0.0f, 1.0f]

	sf::Time m_TotalTime;
	sf::Time m_TimeElapsed;

	TransitionData m_StartData;
	TransitionData m_EndData;

	EaseType m_EaseType;
	//bool m_IsActive; // True while we are still transitioning

};
