#pragma once

#include <SFML\System\Vector2.hpp>
#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\Transformable.hpp>

struct TransitionData
{
	sf::Transformable transformable;
	sf::Color color;
};

class ApexTransition
{
public:
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

	ApexTransition();
	ApexTransition(TransitionData start, TransitionData end, sf::Time totalTime, EaseType easeType);
	virtual ~ApexTransition();

	void Create(TransitionData start, TransitionData end, sf::Time totalTime, EaseType easeType);

	void Tick(sf::Time elapsed);
	TransitionData GetCurrentTransitionData();

	void Restart();
	void Swap(); // Swap start and end data, and reset
	void SetEaseType(EaseType easeType);
	void SetFinished();

private:
	sf::Vector2f Lerp(sf::Vector2f start, sf::Vector2f delta, float t);
	float Lerp(float start, float delta, float t); // t must be in range [0.0f, 1.0f]

	sf::Time m_TotalTime;
	sf::Time m_TimeElapsed;

	TransitionData m_StartData;
	TransitionData m_EndData;

	EaseType m_EaseType;

};
