#include "nfaparser.h"
#include <stack>

namespace Regex
{
    NfaParser::NfaParser(const std::string& rawInfixExpression)
    {
		std::string concatinatedInfixExpression = addConcatinations(rawInfixExpression);
        std::string postfixExpression = infixToPostfix(concatinatedInfixExpression);
        _nfa = generateNfa(postfixExpression);
    }

    std::pair<NfaParser::State, NfaParser::State> NfaParser::getNfa()
    {
        return _nfa;
    }

    static bool isOperator(char ch)
    {
        if (ch == '*' || ch == '.' || ch == '|') 
            return true;
        return false;
    }

    static bool isOperand(char ch)
    {
        return !isOperator(ch) && ch != '(' && ch != ')';
    }

    std::string NfaParser::addConcatinations(const std::string& rawInfixExpression)
    {
        std::string modifiedExpression;
        for (int index = 0; index < rawInfixExpression.length(); index++)
        {
            auto currentChar = rawInfixExpression[index];
            modifiedExpression += currentChar;
            if (index < rawInfixExpression.length() - 1)
            {
                auto nextChar = rawInfixExpression[index + 1];
                bool insertConcatination = (currentChar == ')' || currentChar == '*' || isOperand(currentChar)) && (nextChar == '(' || isOperand(nextChar));
                if (insertConcatination)
                {
                    modifiedExpression += '.';
                }
            }
        }
        return modifiedExpression;
    }

	static int precedence(char op)
	{
        switch (op)
        {
		case '*': return 3;
		case '.': return 2;
		case '|': return 1;
        default : return 0;
        }
	}

    std::string NfaParser::infixToPostfix(const std::string& infixExpression)
    {
        std::string postfixExpression;
		std::stack<char> operatorStack;
        for (auto ch : infixExpression)
        {
            if(isOperand(ch))
				postfixExpression += ch;
            else if (ch == '(')
                operatorStack.push(ch);
            else if (ch == ')')
            {
				while (!operatorStack.empty() && operatorStack.top() != '(')
				{
					postfixExpression += operatorStack.top();
					operatorStack.pop();
				}
                if (!operatorStack.empty())
                    operatorStack.pop();
            }
            if (isOperator(ch))
            {
				while (!operatorStack.empty() && operatorStack.top() != '(')
				{
					auto topOperator = operatorStack.top();
					if (precedence(topOperator) > precedence(ch) || (precedence(topOperator) == precedence(ch) && ch != '*'))
					{
						postfixExpression += topOperator;
						operatorStack.pop();
					}
					else
						break;
					postfixExpression += operatorStack.top();
					operatorStack.pop();
				}
				operatorStack.push(ch);
            }
        }
		while (!operatorStack.empty())
		{
			postfixExpression += operatorStack.top();
			operatorStack.pop();
		}
        return postfixExpression;
    }

    NfaParser::State NfaParser::createState(const bool& isEnd)
    {
		State state;
		state.isEnd = isEnd;
        return state;
    }

    std::pair<NfaParser::State, NfaParser::State> NfaParser::generateNfa(const std::string& postfixExpression)
    {
        if (postfixExpression.empty())
            return createEpsilonTransition();

		std::stack<std::pair<State,State>> stateStack;

		for (char ch : postfixExpression)
		{
            if (ch == '*')
            {
				stateStack.push(kleeneStarState(stateStack.top()));
                stateStack.pop();
            }
            else if (ch == '|')
            {
				auto right = stateStack.top();
				stateStack.pop();
				auto left = stateStack.top();
				stateStack.pop();
				stateStack.push(unionStates(left, right));
			}
			else if (ch == '.')
			{
				auto right = stateStack.top();
				stateStack.pop();
				auto left = stateStack.top();
				stateStack.pop();
				stateStack.push(concatStates(left, right));
            }
            else
            {
				stateStack.push(createSymbolTransition(ch));
            }
		}
		return stateStack.top();
    }

    void NfaParser::addEpsilonTransition(State& from, State& to)
    {
		from.epsilonTransitions.push_back(to);
    }

    void NfaParser::addSymbolTransition(State& from, char symbol, State& to)
    {
		from.symbolTransitions[symbol] = to;
    }

    std::pair<NfaParser::State, NfaParser::State> NfaParser::createEpsilonTransition()
    {
        State start = createState(false);
        State end = createState(true);
		addEpsilonTransition(start, end);
        return std::make_pair(start, end);
    }

    std::pair<NfaParser::State, NfaParser::State> NfaParser::createSymbolTransition(char symbol)
    {
		State start = createState(false);
		State end = createState(true);
		addSymbolTransition(start, symbol, end);
        return std::make_pair(start,end);
    }
    
    std::pair<NfaParser::State, NfaParser::State> NfaParser::concatStates(std::pair<State, State>& first, std::pair<State, State>& second)
    {
		addEpsilonTransition(first.second, second.first);
		first.second.isEnd = false;

        return std::make_pair(first.first,second.second);
    }

    std::pair<NfaParser::State, NfaParser::State> NfaParser::unionStates(std::pair<State, State>& first, std::pair<State, State>& second)
    {
		auto start = createState(false);
		addEpsilonTransition(start, first.first);
		addEpsilonTransition(start, second.first);

		auto end = createState(true);
		addEpsilonTransition(first.second, end);
		addEpsilonTransition(second.second, end);
		first.second.isEnd = false;
		second.second.isEnd = false;

        return std::make_pair(start,end);
    }
    
    std::pair<NfaParser::State, NfaParser::State> NfaParser::kleeneStarState(std::pair<State,State>& nfa)
    {
        auto start = createState(false);
        auto end = createState(true);
		addEpsilonTransition(start, end);
		addEpsilonTransition(start, nfa.first);

		addEpsilonTransition(nfa.second, end);
		addEpsilonTransition(nfa.second, nfa.first);
		nfa.second.isEnd = false;

		return std::make_pair(start, end);
    }
}