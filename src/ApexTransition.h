#pragma once

#include <SFML\System\Vector2.hpp>
#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\Transformable.hpp>

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
	ApexTransition(sf::Time totalTime, EaseType easeType);
	virtual ~ApexTransition();

	virtual void Create(sf::Time totalTime, EaseType easeType);

	virtual void Tick(sf::Time elapsed);

	virtual void SwapAndRestart() = 0; 
	virtual void Restart();
	virtual void SetEaseType(EaseType easeType);
	virtual void SetDuration(sf::Time duration);
	virtual void SetFinished();
	virtual bool IsFinished() const;

	float GetPercentComplete() const;

protected:
	sf::Vector2f Lerp(sf::Vector2f start, sf::Vector2f delta, float t) const;
	int Lerp(int start, int delta, float t) const;
	float Lerp(float start, float delta, float t) const; // t must be in range [0.0f, 1.0f]

	sf::Time m_TotalTime;
	sf::Time m_TimeElapsed;

	EaseType m_EaseType;

};
