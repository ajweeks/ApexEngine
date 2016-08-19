
#include "IntTransition.h"

#include <assert.h>
#include <stdexcept>

IntTransition::IntTransition()
{
}

IntTransition::IntTransition(int start, int end, sf::Time totalTime, EaseType easeType) :
	ApexTransition(totalTime, easeType),
	m_StartValue(start), m_EndValue(end)
{
}

IntTransition::~IntTransition()
{
}

void IntTransition::Create(int start, int end, sf::Time totalTime, EaseType easeType)
{
	ApexTransition::Create(totalTime, easeType);
	m_StartValue = start;
	m_EndValue = end;
}

void IntTransition::Tick(sf::Time elapsed)
{
	m_TimeElapsed += elapsed;
	if (m_TimeElapsed > m_TotalTime)
	{
		return;
	}
}

int IntTransition::GetCurrentTransitionData()
{
	int result(m_StartValue);

	const float progress = std::min(m_TimeElapsed.asSeconds() / m_TotalTime.asSeconds(), 1.0f);

	if (progress == 1.0f) // No need to lerp anything
	{
		return m_EndValue;
	}

	if (m_EndValue != m_StartValue) result = (Lerp(m_StartValue, m_EndValue - m_StartValue, progress));

	return result;
}

void IntTransition::SwapAndRestart()
{
	int tempValue = m_StartValue;
	m_StartValue = m_EndValue;
	m_EndValue = tempValue;
	m_TimeElapsed = sf::Time::Zero;
}

