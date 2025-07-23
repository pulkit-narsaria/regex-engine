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

        std::pair<State, State> getNfa();

    private:
        std::string addConcatinations(const std::string& rawInfixExpression);
        std::string infixToPostfix(const std::string &infixExpression);
        State createState(const bool &isEnd);
        std::pair<State, State> generateNfa(const std::string &postfixExpression);
		void addEpsilonTransition(State& from, State& to);
		void addSymbolTransition(State& from, char symbol, State& to);
        std::pair<State, State> createEpsilonTransition();
        std::pair<State, State> createSymbolTransition(char symbol);
		std::pair<State, State> concatStates(std::pair<State, State>& first, std::pair<State, State>& second);
		std::pair<State, State> unionStates(std::pair<State, State>& first, std::pair<State, State>& second);
		std::pair<State, State> kleeneStarState(std::pair<State, State>& state);
        std::pair<State, State> _nfa;
    };
}