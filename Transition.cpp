
#include "Transition.h"
#include <assert.h>
#include <stdexcept>

Transition::Transition()
{
}

Transition::Transition(TransitionData start, TransitionData end, sf::Time totalTime, EaseType easeType) :
	m_StartData(start), m_EndData(end), m_TotalTime(totalTime), m_EaseType(easeType)
{
	assert(totalTime.asSeconds() > 0.0f);
}

Transition::~Transition()
{
}

void Transition::Create(TransitionData start, TransitionData end, sf::Time totalTime, EaseType easeType)
{
	m_StartData = start;
	m_EndData = end;
	m_TotalTime = totalTime;
	m_EaseType = easeType;
}

void Transition::Tick(sf::Time elapsed)
{
	m_TimeElapsed += elapsed;
	if (m_TimeElapsed > m_TotalTime)
	{
		//m_IsActive = false;
		return;
	}
}

TransitionData Transition::GetCurrentTransitionData()
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
		result.color.r = sf::Uint8(Lerp(startColor.r, float(m_EndData.color.r - startColor.r), progress));
		result.color.g = sf::Uint8(Lerp(startColor.g, float(m_EndData.color.g - startColor.g), progress));
		result.color.b = sf::Uint8(Lerp(startColor.b, float(m_EndData.color.b - startColor.b), progress));
		result.color.a = sf::Uint8(Lerp(startColor.a, float(m_EndData.color.a - startColor.a), progress));
	}

	return result;
}

sf::Vector2f Transition::Lerp(sf::Vector2f start, sf::Vector2f delta, float t)
{
	return sf::Vector2f(Lerp(start.x, delta.x, t), Lerp(start.y, delta.y, t));
}

float Transition::Lerp(float start, float delta, float t)
{
	assert(t >= 0.0f && t <= 1.0f);

	switch (m_EaseType)
	{
	case EaseType::LINEAR:
	{
		return start + t * delta;
	} break;
	case EaseType::QUADRATIC_IN:
	{
		return start + t * t * delta;
	} break;
	case EaseType::QUADRATIC_OUT:
	{
		return start + t * (t - 2) * (-delta);
	} break;
	case EaseType::QUADRATIC_IN_OUT:
	{
		t *= 2.0f;
		if (t < 1.0f) return start + t * t * (delta / 2.0f);
		--t;
		return start + (t * (t - 2) - 1) * (-delta / 2.0f);
	} break;
	case EaseType::CIRCULAR_IN:
	{
		return start + (std::sqrt(1.0f - t * t) - 1) * (-delta);
	} break;
	case EaseType::CIRCULAR_OUT:
	{
		--t;
		return start + std::sqrt(1.0f - t * t) * delta;
	} break;
	case EaseType::CIRCULAR_IN_OUT:
	{
		t *= 2.0f;
		if (t < 1.0f) return start + (std::sqrt(1.0f - t * t) - 1) * (-delta / 2.0f);
		t -= 2;
		return start + (std::sqrt(1.0f - t * t) + 1) * (delta / 2.0f);
	} break;
	case EaseType::EXPONENTIAL_IN:
	{
		return start + (std::pow(2.0f, 10 * (t - 1))) * delta;
	} break;
	case EaseType::EXPONENTIAL_OUT:
	{
		return start + (-std::pow(2.0f, -10 * t) + 1) * delta;
	} break;
	case EaseType::EXPONENTIAL_IN_OUT:
	{
		t *= 2.0f;
		if (t < 1.0f) return start + (std::pow(2.0f, 10 * (t - 1))) * (delta / 2.0f);
		--t;
		return start + (-std::pow(2.0f, -10 * t) + 2) * (delta / 2.0f);
	} break;
	default:
	{
		throw std::logic_error("Unhandled ease type!");
		return 0.0f;
	} break;
	}
}

void Transition::Restart()
{
	//m_IsActive = true;
	m_TimeElapsed = sf::Time::Zero;
}

void Transition::Swap()
{
	TransitionData tempData = m_StartData;
	m_StartData = m_EndData;
	m_EndData = tempData;
	m_TimeElapsed = sf::Time::Zero;
}

void Transition::SetEaseType(EaseType easeType)
{
	m_EaseType = easeType;
}
