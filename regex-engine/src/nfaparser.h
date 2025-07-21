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
        
        struct NFA
        {
            bool isEnd;
            std::vector<NFA> epsilonTransitions;
            std::unordered_map<char, NFA> symbolTransitions;
        };

        NFA getNfa();
        
    private:
        std::string infixToPostfix(const std::string &infixExpression);
        NFA createNfaObject(const bool &isEnd);
        NFA generateNfa(const std::string &postfixExpression);
        NFA _nfa;
    };
}