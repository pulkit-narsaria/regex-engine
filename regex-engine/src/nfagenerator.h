#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace Regex
{
	class NfaGenerator
	{
	public:
		NfaGenerator(const std::string& infixExpression);

		struct State
		{
			bool isEnd;
			std::vector<std::shared_ptr<State>> epsilonTransitions;
			std::unordered_map<char, std::shared_ptr<State>> symbolTransitions;
		};

		struct NFA
		{
			std::shared_ptr<State> start;
			std::shared_ptr<State> end;
		};

		NFA getNfa();

	private:
		std::shared_ptr<State> createState(const bool isEnd);
		std::unique_ptr<NFA> generateNfa(const std::string& postfixExpression);
		void addEpsilonTransition(const std::shared_ptr<State> from, const std::shared_ptr<State> to);
		void addSymbolTransition(const std::shared_ptr<State> from, const char symbol, const std::shared_ptr<State> to);
		std::unique_ptr<NFA> createEpsilonTransition();
		std::unique_ptr<NFA> createSymbolTransition(const char symbol);
		std::unique_ptr<NFA> concatStates(const std::unique_ptr<NFA> first, const std::unique_ptr<NFA> second);
		std::unique_ptr<NFA> unionStates(const std::unique_ptr<NFA> first, const std::unique_ptr<NFA> second);
		std::unique_ptr<NFA> kleeneStarState(const std::unique_ptr<NFA> state);
		NFA _nfa;
	};
}