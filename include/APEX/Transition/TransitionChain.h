#pragma once

#include "Apex\Transition\Transition.h"

#include <vector>

namespace apex
{
	class TransitionChain
	{
	public:
		TransitionChain();
		TransitionChain(std::vector<std::pair<std::string, Transition*>> transitions);
		virtual ~TransitionChain();

		TransitionChain(const TransitionChain&) = delete;
		TransitionChain& operator=(const TransitionChain&) = delete;

		void AddTransition(Transition* transition, std::string name);

		void Tick(sf::Time elapsed);

		bool IsFinished() const;
		std::pair<std::string, Transition*> GetCurrentTransition();
		size_t GetNumberOfTransitions() const;

		void Clear();

	private:
		std::vector<std::pair<std::string, Transition*>> m_Transitions;
		size_t m_CurrentTransitionIndex;

		bool m_IsFinished;

	};
} // namespace apex