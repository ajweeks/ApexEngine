#pragma once

#include "ApexTransition.h"

#include <SFML\System\Vector2.hpp>
#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\Transformable.hpp>

class Vector2fTransition : public ApexTransition
{
public:
	Vector2fTransition();
	Vector2fTransition(sf::Vector2f start, sf::Vector2f end, sf::Time totalTime, EaseType easeType = EaseType::LINEAR);
	virtual ~Vector2fTransition();

	void Create(sf::Vector2f start, sf::Vector2f end, sf::Time totalTime, EaseType easeType = EaseType::LINEAR);

	virtual void Tick(sf::Time elapsed) override;
	virtual sf::Vector2f GetCurrentTransitionData() const;

	virtual void SwapAndRestart() override; // Swap start and end data, and reset

private:
	sf::Vector2f m_StartVector;
	sf::Vector2f m_EndVector;

};
