
#include "ColorTransition.h"

#include <assert.h>
#include <stdexcept>

ColorTransition::ColorTransition()
{
}

ColorTransition::ColorTransition(sf::Color start, sf::Color end, sf::Time totalTime, EaseType easeType) :
	ApexTransition(totalTime, easeType),
	m_StartColor(start), m_EndColor(end)
{
	assert(totalTime.asSeconds() > 0.0f);
}

ColorTransition::~ColorTransition()
{
}

void ColorTransition::Create(sf::Color start, sf::Color end, sf::Time totalTime, EaseType easeType)
{
	ApexTransition::Create(totalTime, easeType);
	m_StartColor = start;
	m_EndColor = end;
}

void ColorTransition::Tick(sf::Time elapsed)
{
	m_TimeElapsed += elapsed;
	if (m_TimeElapsed > m_TotalTime)
	{
		return;
	}
}

sf::Color ColorTransition::GetCurrentColor()
{
	sf::Color result(m_StartColor);

	const float progress = std::min(m_TimeElapsed.asSeconds() / m_TotalTime.asSeconds(), 1.0f);

	if (progress == 1.0f) // No need to lerp anything
	{
		result = sf::Color(m_EndColor);
		return result;
	}

	const sf::Color startColor = m_StartColor;
	if (m_EndColor != m_StartColor)
	{
		result.r = sf::Uint8(Lerp(startColor.r, m_EndColor.r - startColor.r, progress));
		result.g = sf::Uint8(Lerp(startColor.g, m_EndColor.g - startColor.g, progress));
		result.b = sf::Uint8(Lerp(startColor.b, m_EndColor.b - startColor.b, progress));
		result.a = sf::Uint8(Lerp(startColor.a, m_EndColor.a - startColor.a, progress));
	}

	return result;
}

void ColorTransition::SwapAndRestart()
{
	sf::Color tempData = m_StartColor;
	m_StartColor = m_EndColor;
	m_EndColor = tempData;
	m_TimeElapsed = sf::Time::Zero;
}

