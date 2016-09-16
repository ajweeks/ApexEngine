#pragma once

#include "ApexTransition.h"

#include <vector>

class TransitionChain
{
public:
	TransitionChain();
	TransitionChain(std::vector<std::pair<std::string, ApexTransition*>> transitions);
	virtual ~TransitionChain();

	TransitionChain(const TransitionChain&) = delete;
	TransitionChain& operator=(const TransitionChain&) = delete;

	void AddTransition(ApexTransition* transition, std::string name);

	void Tick(sf::Time elapsed);

	bool IsFinished() const;
	std::pair<std::string, ApexTransition*> GetCurrentTransition();
	size_t GetNumberOfTransitions() const;

	void Clear();

private:
	std::vector<std::pair<std::string, ApexTransition*>> m_Transitions;
	size_t m_CurrentTransitionIndex;

	bool m_IsFinished;

};
