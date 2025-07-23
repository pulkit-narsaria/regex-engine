#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace Regex
{
	class NfaParser
	{
	public:
		NfaParser(const std::string& infixExpression);

		struct State
		{
			bool isEnd;
			std::vector<State> epsilonTransitions;
			std::unordered_map<char, State> symbolTransitions;
		};

		struct NFA
		{
			State start;
			State end;
		};

		NFA getNfa();

	private:
		std::string addConcatenations(const std::string& rawInfixExpression);
		std::string infixToPostfix(const std::string& infixExpression);
		State createState(const bool& isEnd);
		NFA generateNfa(const std::string& postfixExpression);
		void addEpsilonTransition(State& from, State& to);
		void addSymbolTransition(State& from, char symbol, State& to);
		NFA createEpsilonTransition();
		NFA createSymbolTransition(char symbol);
		NFA concatStates(NFA& first, NFA& second);
		NFA unionStates(NFA& first, NFA& second);
		NFA kleeneStarState(NFA& state);
		NFA _nfa;
	};
}