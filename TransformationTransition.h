#pragma once

#include "ApexTransition.h"

#include <SFML\System\Vector2.hpp>
#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\Transformable.hpp>

struct TransitionData
{
	sf::Transformable transformable;
	sf::Color color;
};

class TransformationTransition : public ApexTransition
{
public:
	TransformationTransition();
	TransformationTransition(TransitionData start, TransitionData end, sf::Time totalTime, EaseType easeType);
	virtual ~TransformationTransition();

	void Create(TransitionData start, TransitionData end, sf::Time totalTime, EaseType easeType);

	virtual void Tick(sf::Time elapsed) override;
	virtual TransitionData GetCurrentTransitionData();

	virtual void Swap() override; // Swap start and end data, and reset

private:
	TransitionData m_StartData;
	TransitionData m_EndData;

};
