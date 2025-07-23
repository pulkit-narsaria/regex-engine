#include "nfaparser.h"
#include <stack>

namespace Regex
{
	NfaParser::NfaParser(const std::string& rawInfixExpression)
	{
		std::string concatenatedInfixExpression = addConcatenations(rawInfixExpression);
		std::string postfixExpression = infixToPostfix(concatenatedInfixExpression);
		_nfa = generateNfa(postfixExpression);
	}

	NfaParser::NFA NfaParser::getNfa()
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

	std::string NfaParser::addConcatenations(const std::string& rawInfixExpression)
	{
		std::string modifiedExpression;
		for (size_t index = 0; index < rawInfixExpression.length(); index++)
		{
			auto currentChar = rawInfixExpression[index];
			modifiedExpression += currentChar;
			if (index < rawInfixExpression.length() - 1)
			{
				auto nextChar = rawInfixExpression[index + 1];
				bool insertConcatenation = (currentChar == ')' || currentChar == '*' || isOperand(currentChar)) && (nextChar == '(' || isOperand(nextChar));
				if (insertConcatenation)
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
		default: return 0;
		}
	}

	std::string NfaParser::infixToPostfix(const std::string& infixExpression)
	{
		std::string postfixExpression;
		std::stack<char> operatorStack;
		for (auto ch : infixExpression)
		{
			if (isOperand(ch))
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

	NfaParser::NFA NfaParser::generateNfa(const std::string& postfixExpression)
	{
		if (postfixExpression.empty())
			return createEpsilonTransition();

		std::stack<NFA> stateStack;

		for (char ch : postfixExpression)
		{
			if (ch == '*')
			{
				auto nfa = stateStack.top();
				stateStack.pop();
				stateStack.push(kleeneStarState(nfa));
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

	NfaParser::NFA NfaParser::createEpsilonTransition()
	{
		State start = createState(false);
		State end = createState(true);
		addEpsilonTransition(start, end);
		return NFA{ start,end };
	}

	NfaParser::NFA NfaParser::createSymbolTransition(char symbol)
	{
		State start = createState(false);
		State end = createState(true);
		addSymbolTransition(start, symbol, end);
		return NFA{ start,end };
	}

	NfaParser::NFA NfaParser::concatStates(NFA& first, NFA& second)
	{
		addEpsilonTransition(first.end, second.start);
		first.end.isEnd = false;

		return NFA{ first.start,second.end };
	}

	NfaParser::NFA NfaParser::unionStates(NFA& first, NFA& second)
	{
		auto start = createState(false);
		addEpsilonTransition(start, first.start);
		addEpsilonTransition(start, second.start);

		auto end = createState(true);
		addEpsilonTransition(first.end, end);
		addEpsilonTransition(second.end, end);
		first.end.isEnd = false;
		second.end.isEnd = false;

		return NFA{ start,end };
	}

	NfaParser::NFA NfaParser::kleeneStarState(NFA& nfa)
	{
		auto start = createState(false);
		auto end = createState(true);
		addEpsilonTransition(start, end);
		addEpsilonTransition(start, nfa.start);

		addEpsilonTransition(nfa.end, end);
		addEpsilonTransition(nfa.end, nfa.start);
		nfa.end.isEnd = false;

		return NFA{ start, end };
	}
}