
#include "Apex\Transition\TransformableTransition.h"

#include <assert.h>
#include <stdexcept>

namespace apex
{
	TransformableTransition::TransformableTransition()
	{
	}

	TransformableTransition::TransformableTransition(sf::Transformable start, sf::Transformable end, sf::Time totalTime, EaseType easeType) :
		Transition(totalTime, easeType),
		m_StartTransformable(start), m_EndTransformable(end)
	{
	}

	TransformableTransition::~TransformableTransition()
	{
	}

	void TransformableTransition::Create(sf::Transformable start, sf::Transformable end, sf::Time totalTime, EaseType easeType)
	{
		Transition::Create(totalTime, easeType);
		m_StartTransformable = start;
		m_EndTransformable = end;

	}

	void TransformableTransition::Tick(sf::Time elapsed)
	{
		m_TimeElapsed += elapsed;
		if (m_TimeElapsed > m_TotalTime)
		{
			return;
		}
	}

	sf::Transformable TransformableTransition::GetCurrentTransformable()
	{
		sf::Transformable result(m_StartTransformable);

		if (m_TimeElapsed == sf::Time::Zero)
		{
			return m_StartTransformable;
		}
		if (m_TotalTime == sf::Time::Zero)
		{
			return m_EndTransformable;
		}

		const float progress = std::min(m_TimeElapsed.asSeconds() / m_TotalTime.asSeconds(), 1.0f);

		if (progress == 1.0f) // No need to lerp anything
		{
			return m_EndTransformable;
		}

		const sf::Vector2f startPos = m_StartTransformable.getPosition();
		const sf::Vector2f deltaPos = m_EndTransformable.getPosition() - startPos;
		if (deltaPos != sf::Vector2f(0.0f, 0.0f)) result.setPosition(Lerp(startPos, deltaPos, progress));

		const float startRotation = m_StartTransformable.getRotation();
		const float deltaRotation = m_EndTransformable.getRotation() - startRotation;
		if (deltaRotation != 0.0f) result.setRotation(Lerp(startRotation, deltaRotation, progress));

		const sf::Vector2f startScale = m_StartTransformable.getScale();
		const sf::Vector2f deltaScale = m_EndTransformable.getScale() - startScale;
		if (deltaScale != sf::Vector2f(0.0f, 0.0f)) result.setScale(Lerp(startScale, deltaScale, progress));

		return result;
	}

	void TransformableTransition::SwapAndRestart()
	{
		sf::Transformable tempData = m_StartTransformable;
		m_StartTransformable = m_EndTransformable;
		m_EndTransformable = tempData;
		m_TimeElapsed = sf::Time::Zero;
	}

	sf::Transformable TransformableTransition::GetStartTransformable() const
	{
		return m_StartTransformable;
	}

	sf::Transformable TransformableTransition::GetEndTransformable() const
	{
		return m_EndTransformable;
	}
} // namespace apex
