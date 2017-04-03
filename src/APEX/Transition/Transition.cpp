
#include "Apex\Transition\Transition.h"

#include <assert.h>
#include <stdexcept>

namespace apex
{
	Transition::Transition()
	{
	}

	Transition::Transition(sf::Time totalTime, EaseType easeType) :
		m_TotalTime(totalTime), m_EaseType(easeType)
	{
		assert(totalTime.asSeconds() > 0.0f);
	}

	Transition::~Transition()
	{
	}

	void Transition::Create(sf::Time totalTime, EaseType easeType)
	{
		m_TotalTime = totalTime;
		m_EaseType = easeType;
		m_TimeElapsed = sf::Time::Zero;
	}

	sf::Vector2f Transition::Lerp(sf::Vector2f start, sf::Vector2f delta, float t) const
	{
		return sf::Vector2f(Lerp(start.x, delta.x, t), Lerp(start.y, delta.y, t));
	}

	int Transition::Lerp(int start, int delta, float t) const
	{
		return int(Lerp(float(start), float(delta), t));
	}

	float Transition::Lerp(float start, float delta, float t) const
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
		m_TimeElapsed = sf::Time::Zero;
	}

	void Transition::SetEaseType(EaseType easeType)
	{
		m_EaseType = easeType;
	}

	void Transition::SetDuration(sf::Time duration)
	{
		m_TotalTime = duration;
	}

	void Transition::SetFinished()
	{
		m_TimeElapsed = m_TotalTime;
	}

	bool Transition::IsFinished() const
	{
		return m_TimeElapsed >= m_TotalTime;
	}

	float Transition::GetPercentComplete() const
	{
		if (m_TotalTime == sf::Time::Zero) return 0.0f;
		return m_TimeElapsed / m_TotalTime;
	}
} // namespace apex
