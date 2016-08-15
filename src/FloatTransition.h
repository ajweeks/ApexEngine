#pragma once

#include "ApexTransition.h"

#include <SFML\System\Vector2.hpp>
#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\Transformable.hpp>

class FloatTransition : public ApexTransition
{
public:
	FloatTransition();
	FloatTransition(float start, float end, sf::Time totalTime, EaseType easeType = EaseType::LINEAR);
	virtual ~FloatTransition();

	void Create(float start, float end, sf::Time totalTime, EaseType easeType = EaseType::LINEAR);

	virtual void Tick(sf::Time elapsed) override;
	virtual float GetCurrentTransitionData();

	virtual void Swap() override; // Swap start and end data, and reset

private:
	float m_StartValue;
	float m_EndValue;

};
