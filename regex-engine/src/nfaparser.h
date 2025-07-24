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
		std::string addConcatenations(const std::string& rawInfixExpression);
		std::string infixToPostfix(const std::string& infixExpression);
		std::shared_ptr<State> createState(const bool& isEnd);
		std::unique_ptr<NFA> generateNfa(const std::string& postfixExpression);
		void addEpsilonTransition(std::shared_ptr<State> from, std::shared_ptr<State> to);
		void addSymbolTransition(std::shared_ptr<State> from, char& symbol, std::shared_ptr<State> to);
		std::unique_ptr<NFA> createEpsilonTransition();
		std::unique_ptr<NFA> createSymbolTransition(char symbol);
		std::unique_ptr<NFA> concatStates(std::unique_ptr<NFA> first, std::unique_ptr<NFA> second);
		std::unique_ptr<NFA> unionStates(std::unique_ptr<NFA> first, std::unique_ptr<NFA> second);
		std::unique_ptr<NFA> kleeneStarState(std::unique_ptr<NFA> state);
		//bool recursiveBacktrackingSearch(const std::string& input, const State& currentState, size_t position, std::vector<std::shared_ptr<State>> visited);
		bool search(std::unique_ptr<NFA> nfa, std::string& word);
		void addNextState(std::shared_ptr<State> currentState, std::vector<std::shared_ptr<State>>& nextStates, std::vector<std::shared_ptr<State>>& visitedStates);
		NFA _nfa;
	};
}