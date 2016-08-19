
#include "Vector2fTransition.h"

#include <assert.h>
#include <stdexcept>

Vector2fTransition::Vector2fTransition()
{
}

Vector2fTransition::Vector2fTransition(sf::Vector2f start, sf::Vector2f end, sf::Time totalTime, EaseType easeType) :
	ApexTransition(totalTime, easeType),
	m_StartVector(start), m_EndVector(end)
{
}

Vector2fTransition::~Vector2fTransition()
{
}

void Vector2fTransition::Create(sf::Vector2f start, sf::Vector2f end, sf::Time totalTime, EaseType easeType)
{
	ApexTransition::Create(totalTime, easeType);
	m_StartVector = start;
	m_EndVector = end;
}

void Vector2fTransition::Tick(sf::Time elapsed)
{
	m_TimeElapsed += elapsed;
	if (m_TimeElapsed > m_TotalTime)
	{
		return;
	}
}

sf::Vector2f Vector2fTransition::GetCurrentTransitionData() const
{
	sf::Vector2f result(m_StartVector);

	const float progress = std::min(m_TimeElapsed.asSeconds() / m_TotalTime.asSeconds(), 1.0f);

	if (progress == 1.0f) // No need to lerp anything
	{
		return m_EndVector;
	}

	if (m_EndVector != m_StartVector) result = (Lerp(m_StartVector, m_EndVector - m_StartVector, progress));

	return result;
}

void Vector2fTransition::SwapAndRestart()
{
	sf::Vector2f tempValue = m_StartVector;
	m_StartVector = m_EndVector;
	m_EndVector = tempValue;
	m_TimeElapsed = sf::Time::Zero;
}

