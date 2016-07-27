#pragma once

#include "ApexTransition.h"

#include <SFML\System\Vector2.hpp>
#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\Transformable.hpp>

class IntTransition : public ApexTransition
{
public:
	IntTransition();
	IntTransition(int start, int end, sf::Time totalTime, EaseType easeType = EaseType::LINEAR);
	virtual ~IntTransition();

	void Create(int start, int end, sf::Time totalTime, EaseType easeType = EaseType::LINEAR);

	virtual void Tick(sf::Time elapsed) override;
	virtual int GetCurrentTransitionData();

	virtual void Swap() override; // Swap start and end data, and reset

private:
	int m_StartValue;
	int m_EndValue;

};
