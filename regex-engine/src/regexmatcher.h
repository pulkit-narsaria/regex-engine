#pragma once
#include "nfagenerator.h"
namespace Regex
{
	class RegexMatcher
	{
	public:
		RegexMatcher(const std::string& infixExpression);
		//bool recursiveBacktrackingSearch(const std::string& input, const State& currentState, size_t position, std::vector<std::shared_ptr<State>> visited);
		bool search(const std::string& word);
	private:
		void addNextState(const std::shared_ptr<NfaGenerator::State> currentState, std::vector<std::shared_ptr<NfaGenerator::State>>& nextStates, std::vector<std::shared_ptr<NfaGenerator::State>>& visitedStates);
		NfaGenerator _nfa;
	};
}