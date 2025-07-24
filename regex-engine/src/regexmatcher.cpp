#include "regexmatcher.h"

namespace Regex
{
	RegexMatcher::RegexMatcher(const std::string& infixExpression)
		: _nfa(infixExpression)
	{
	}

	void RegexMatcher::addNextState(const std::shared_ptr<NfaGenerator::State> currentState, std::vector<std::shared_ptr<NfaGenerator::State>>& nextStates, std::vector<std::shared_ptr<NfaGenerator::State>>& visitedStates)
	{
		if (!currentState->epsilonTransitions.empty())
		{
			for (auto nextState : currentState->epsilonTransitions)
			{
				if (std::find(visitedStates.begin(), visitedStates.end(), nextState) == visitedStates.end())
				{
					visitedStates.push_back(nextState);
					addNextState(nextState, nextStates, visitedStates);
				}
			}
		}
		else
		{
			nextStates.push_back(currentState);
		}
	}

	bool RegexMatcher::search(const std::string& word)
	{
		auto nfa = _nfa.getNfa();
		std::vector<std::shared_ptr<NfaGenerator::State>> currentStates;
		std::vector<std::shared_ptr<NfaGenerator::State>> visitedStates;
		addNextState(nfa.start, currentStates, visitedStates);
		for (auto ch : word)
		{
			std::vector<std::shared_ptr<NfaGenerator::State>> nextStates;
			for (auto& state : currentStates)
			{
				if (state->symbolTransitions.find(ch) != state->symbolTransitions.end())
				{
					auto nextState = state->symbolTransitions.at(ch);
					visitedStates.clear();
					addNextState(nextState, nextStates, visitedStates);
				}
			}
			currentStates = nextStates;
		}
		for (auto state : currentStates)
		{
			if (state->isEnd)
				return true;
		}
		return false;
	}

	/*bool NfaGenerator::recursiveBacktrackingSearch(const std::string& input, const State& currentState, size_t position, std::vector<std::shared_ptr<State>> visited)
	{
		if (std::find(visited.begin(), visited.end(), currentState) != visited.end())
			return false;

		visited.push_back(currentState);

		if (position == input.length())
		{
			if (currentState.isEnd)
				return true;
			for (const auto& nextState : currentState.epsilonTransitions)
			{
				if (recursiveBacktrackingSearch(input, nextState, position, visited))
					return true;
			}
		}
		else
		{
			if (currentState.symbolTransitions.find(input[position]) == currentState.symbolTransitions.end())
			{
				for (const auto& nextState : currentState.epsilonTransitions)
				{
					if (recursiveBacktrackingSearch(input, nextState, position, visited))
						return true;
				}
			}
			else
			{
				std::vector<std::shared_ptr<State>> tempVisited;
				if (recursiveBacktrackingSearch(input, currentState.symbolTransitions.at(input[position]), position + 1, tempVisited))
					return true;
			}
		}
		return false;
	}*/
}