
#include <APEX\Transition\FloatTransition.h>

#include <assert.h>
#include <stdexcept>

namespace apex
{
	FloatTransition::FloatTransition()
	{
	}

	FloatTransition::FloatTransition(float start, float end, sf::Time totalTime, EaseType easeType) :
		Transition(totalTime, easeType),
		m_StartValue(start), m_EndValue(end)
	{
	}

	FloatTransition::~FloatTransition()
	{
	}

	void FloatTransition::Create(float start, float end, sf::Time totalTime, EaseType easeType)
	{
		Transition::Create(totalTime, easeType);
		m_StartValue = start;
		m_EndValue = end;
	}

	void FloatTransition::Tick(sf::Time elapsed)
	{
		m_TimeElapsed += elapsed;
		if (m_TimeElapsed > m_TotalTime)
		{
			return;
		}
	}

	float FloatTransition::GetCurrentTransitionData()
	{
		float result(m_StartValue);

		const float progress = std::min(m_TimeElapsed.asSeconds() / m_TotalTime.asSeconds(), 1.0f);

		if (progress == 1.0f) // No need to lerp anything
		{
			return m_EndValue;
		}

		if (m_EndValue != m_StartValue) result = (Lerp(m_StartValue, m_EndValue - m_StartValue, progress));

		return result;
	}

	void FloatTransition::SwapAndRestart()
	{
		float tempValue = m_StartValue;
		m_StartValue = m_EndValue;
		m_EndValue = tempValue;
		m_TimeElapsed = sf::Time::Zero;
	}
} // namespace apex
