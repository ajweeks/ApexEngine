
#include "TransformationTransition.h"

#include <assert.h>
#include <stdexcept>

TransformationTransition::TransformationTransition()
{
}

TransformationTransition::TransformationTransition(TransitionData start, TransitionData end, sf::Time totalTime, EaseType easeType) :
	ApexTransition(totalTime, easeType),
	m_StartData(start), m_EndData(end)
{
	assert(totalTime.asSeconds() > 0.0f);
}

TransformationTransition::~TransformationTransition()
{
}

void TransformationTransition::Create(TransitionData start, TransitionData end, sf::Time totalTime, EaseType easeType)
{
	ApexTransition::Create(totalTime, easeType);
	m_StartData = start;
	m_EndData = end;
}

void TransformationTransition::Tick(sf::Time elapsed)
{
	m_TimeElapsed += elapsed;
	if (m_TimeElapsed > m_TotalTime)
	{
		return;
	}
}

TransitionData TransformationTransition::GetCurrentTransitionData()
{
	TransitionData result(m_StartData);

	const float progress = std::min(m_TimeElapsed.asSeconds() / m_TotalTime.asSeconds(), 1.0f);

	if (progress == 1.0f) // No need to lerp anything
	{
		result.transformable.setPosition(m_EndData.transformable.getPosition());
		result.transformable.setRotation(m_EndData.transformable.getRotation());
		result.transformable.setScale(m_EndData.transformable.getScale());
		result.color = sf::Color(m_EndData.color);
		return result;
	}

	const sf::Vector2f startPos = m_StartData.transformable.getPosition();
	const sf::Vector2f deltaPos = m_EndData.transformable.getPosition() - startPos;
	if (deltaPos != sf::Vector2f(0.0f, 0.0f)) result.transformable.setPosition(Lerp(startPos, deltaPos, progress));

	const float startRotation = m_StartData.transformable.getRotation();
	const float deltaRotation = m_EndData.transformable.getRotation() - startRotation;
	if (deltaRotation != 0.0f) result.transformable.setRotation(Lerp(startRotation, deltaRotation, progress));

	const sf::Vector2f startScale = m_StartData.transformable.getScale();
	const sf::Vector2f deltaScale = m_EndData.transformable.getScale() - startScale;
	if (deltaScale != sf::Vector2f(0.0f, 0.0f)) result.transformable.setScale(Lerp(startScale, deltaScale, progress));

	const sf::Color startColor = m_StartData.color;
	if (m_EndData.color != m_StartData.color)
	{
		result.color.r = sf::Uint8(Lerp(startColor.r, m_EndData.color.r - startColor.r, progress));
		result.color.g = sf::Uint8(Lerp(startColor.g, m_EndData.color.g - startColor.g, progress));
		result.color.b = sf::Uint8(Lerp(startColor.b, m_EndData.color.b - startColor.b, progress));
		result.color.a = sf::Uint8(Lerp(startColor.a, m_EndData.color.a - startColor.a, progress));
	}

	return result;
}

void TransformationTransition::Swap()
{
	TransitionData tempData = m_StartData;
	m_StartData = m_EndData;
	m_EndData = tempData;
	m_TimeElapsed = sf::Time::Zero;
}

