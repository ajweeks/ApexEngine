#pragma once

#include "ApexTransition.h"

#include <SFML\System\Vector2.hpp>
#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\Transformable.hpp>

class TransformableTransition : public ApexTransition
{
public:
	TransformableTransition();
	TransformableTransition(sf::Transformable start, sf::Transformable end, sf::Time totalTime, EaseType easeType = EaseType::LINEAR);
	virtual ~TransformableTransition();

	void Create(sf::Transformable start, sf::Transformable end, sf::Time totalTime, EaseType easeType = EaseType::LINEAR);

	virtual void Tick(sf::Time elapsed) override;
	virtual sf::Transformable GetCurrentTransformable();

	virtual void SwapAndRestart() override; // Swap start and end data, and reset

private:
	sf::Transformable m_StartTransformable;
	sf::Transformable m_EndTransformable;

};