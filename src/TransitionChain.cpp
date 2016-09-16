
#include "TransitionChain.h"


TransitionChain::TransitionChain()
{
}

TransitionChain::TransitionChain(std::vector<std::pair<std::string, ApexTransition*>> transitions) :
	m_Transitions(transitions)
{
}

TransitionChain::~TransitionChain()
{
	for (size_t i = 0; i < m_Transitions.size(); i++)
	{
		delete m_Transitions[i].second;
	}
	m_Transitions.clear();
}

void TransitionChain::AddTransition(ApexTransition* transition, std::string name)
{
	m_Transitions.push_back(std::pair<std::string, ApexTransition*>(name, transition));
}

void TransitionChain::Tick(sf::Time elapsed)
{
	if (m_IsFinished) return;

	if (m_CurrentTransitionIndex < m_Transitions.size())
	{
		m_Transitions[m_CurrentTransitionIndex].second->Tick(elapsed);

		if (m_Transitions[m_CurrentTransitionIndex].second->IsFinished())
		{
			if (m_CurrentTransitionIndex < m_Transitions.size() - 1)
			{
				++m_CurrentTransitionIndex;
			}
			else
			{
				m_IsFinished = true;
			}
		}
	}
}

bool TransitionChain::IsFinished() const
{
	return m_IsFinished;
}

std::pair<std::string, ApexTransition*> TransitionChain::GetCurrentTransition()
{
	if (m_CurrentTransitionIndex < m_Transitions.size())
	{
		return m_Transitions[m_CurrentTransitionIndex];
	}

	return std::pair<std::string, ApexTransition*>("", nullptr);
}

size_t TransitionChain::GetNumberOfTransitions() const
{
	return m_Transitions.size();
}

void TransitionChain::Clear()
{
	for (size_t i = 0; i < m_Transitions.size(); i++)
	{
		delete m_Transitions[i].second;
	}
	m_Transitions.clear();
	m_CurrentTransitionIndex = 0;
	m_IsFinished = false;
}
