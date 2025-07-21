#include "nfaparser.h"

namespace Regex
{
    //improvements to be done
	// - take raw infixexpression then add concat operator where needed insted of asking user to give it.

    NfaParser::NfaParser(const std::string& infixExpression)
    {
        std::string postfixExpression = infixToPostfix(infixExpression);
        _nfa = generateNfa(postfixExpression);
    }

    NfaParser::NFA NfaParser::getNfa()
    {
        return _nfa;
    }

    std::string NfaParser::infixToPostfix(const std::string& infixExpression)
    {
        return std::string();
    }

    NfaParser::NFA NfaParser::createNfaObject(const bool& isEnd)
    {
        NFA nfaObject{ isEnd , };
        return nfaObject;
    }

    NfaParser::NFA NfaParser::generateNfa(const std::string& postfixExpression)
    {
        return NFA();
    }
}