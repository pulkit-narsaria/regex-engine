#include "nfaparser.h"

namespace Regex::NfaParser
{
    class NfaParser
    {
        NfaParser(const std::string &infixExpression)
        {
            std::string postfixExpression = infixToPostfix(infixExpression);
            _nfa = generateNfa(postfixExpression);
        }

        NFA getNfa()
        {
            return _nfa;
        }

        std::string NfaParser::infixToPostfix(const std::string& infixExpression)
        {
            return std::string();
        }

        NFA NfaParser::createNfaObject(const bool& isEnd)
        {
            NFA nfaObject { isEnd ,  };
            return nfaObject;
        }

        NFA NfaParser::generateNfa(const std::string& postfixExpression)
        {
            return NFA();
        }
    };
}