#include "nfaparser.h"
#include <stack>

namespace Regex
{
	NfaParser::NfaParser(const std::string& rawInfixExpression)
	{
		std::string concatenatedInfixExpression = addConcatenations(rawInfixExpression);
		std::string postfixExpression = infixToPostfix(concatenatedInfixExpression);
		_nfa = *generateNfa(postfixExpression);
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
			else if (isOperator(ch))
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

	std::shared_ptr<NfaParser::State> NfaParser::createState(const bool isEnd)
	{
		auto state = std::make_shared<State>();
		state->isEnd = isEnd;
		return state;
	}

	std::unique_ptr<NfaParser::NFA> NfaParser::generateNfa(const std::string& postfixExpression)
	{
		if (postfixExpression.empty())
			return createEpsilonTransition();

		std::stack<std::unique_ptr<NFA>> stateStack;

		for (char ch : postfixExpression)
		{
			if (ch == '*')
			{
				auto nfa = std::move(stateStack.top());
				stateStack.pop();
				stateStack.push(kleeneStarState(std::move(nfa)));
			}
			else if (ch == '|')
			{
				auto right = std::move(stateStack.top());
				stateStack.pop();
				auto left = std::move(stateStack.top());
				stateStack.pop();
				stateStack.push(unionStates(std::move(left), std::move(right)));
			}
			else if (ch == '.')
			{
				auto right = std::move(stateStack.top());
				stateStack.pop();
				auto left = std::move(stateStack.top());
				stateStack.pop();
				stateStack.push(concatStates(std::move(left), std::move(right)));
			}
			else
			{
				stateStack.push(createSymbolTransition(ch));
			}
		}
		return std::move(stateStack.top());
	}

	void NfaParser::addEpsilonTransition(const std::shared_ptr<State> from, const std::shared_ptr<State> to)
	{
		from->epsilonTransitions.push_back(to);
	}

	void NfaParser::addSymbolTransition(const std::shared_ptr<State> from,const char symbol,const std::shared_ptr<State> to)
	{
		from->symbolTransitions[symbol] = to;
	}

	std::unique_ptr<NfaParser::NFA> NfaParser::createEpsilonTransition()
	{
		std::shared_ptr<State> start = createState(false);
		std::shared_ptr<State> end = createState(true);
		addEpsilonTransition(start, end);
		return std::make_unique<NFA>(start,end);
	}

	std::unique_ptr<NfaParser::NFA> NfaParser::createSymbolTransition(const char symbol)
	{
		std::shared_ptr<State> start = createState(false);
		std::shared_ptr<State> end = createState(true);
		addSymbolTransition(start, symbol, end);
		return std::make_unique<NFA>(start, end);
	}

	std::unique_ptr<NfaParser::NFA> NfaParser::concatStates(const std::unique_ptr<NFA> first, const std::unique_ptr<NFA> second)
	{
		addEpsilonTransition(first->end, second->start);
		first->end->isEnd = false;
		return std::make_unique<NFA>(first->start,second->end);
	}

	std::unique_ptr<NfaParser::NFA> NfaParser::unionStates(const std::unique_ptr<NFA> first,const std::unique_ptr<NFA> second)
	{
		auto start = createState(false);
		addEpsilonTransition(start, first->start);
		addEpsilonTransition(start, second->start);

		auto end = createState(true);
		addEpsilonTransition(first->end, end);
		addEpsilonTransition(second->end, end);
		first->end->isEnd = false;
		second->end->isEnd = false;

		return std::make_unique<NFA>(start, end);
	}

	std::unique_ptr<NfaParser::NFA> NfaParser::kleeneStarState(const std::unique_ptr<NFA> nfa)
	{
		auto start = createState(false);
		auto end = createState(true);
		addEpsilonTransition(start, end);
		addEpsilonTransition(start, nfa->start);

		addEpsilonTransition(nfa->end, end);
		addEpsilonTransition(nfa->end, nfa->start);
		nfa->end->isEnd = false;

		return std::make_unique<NFA>(start, end);
	}

	/*bool NfaParser::recursiveBacktrackingSearch(const std::string& input, const State& currentState, size_t position, std::vector<std::shared_ptr<State>> visited)
	{
		if (std::find(visited.begin(), visited.end(), currentState) != visited.end())
			return false;
		
		visited.push_back(currentState);
		
		if (position == input.length())
		{
			if (currentState.isEnd)
				return true;
			for (const auto& nextState : currentState.epsilonTransitions)
			{
				if (recursiveBacktrackingSearch(input, nextState, position, visited))
					return true;
			}
		}
		else
		{
			if (currentState.symbolTransitions.find(input[position]) == currentState.symbolTransitions.end())
			{
				for (const auto& nextState : currentState.epsilonTransitions)
				{
					if (recursiveBacktrackingSearch(input, nextState, position, visited))
						return true;
				}
			}
			else
			{
				std::vector<std::shared_ptr<State>> tempVisited;
				if (recursiveBacktrackingSearch(input, currentState.symbolTransitions.at(input[position]), position + 1, tempVisited))
					return true;
			}
		}
		return false;
	}*/

	bool NfaParser::search(const std::unique_ptr<NFA> nfa,const std::string& word)
	{
		std::vector<std::shared_ptr<State>> currentStates;
		std::vector<std::shared_ptr<State>> visitedStates;
		addNextState(nfa->start, currentStates, visitedStates);
		for (auto ch : word)
		{
			std::vector<std::shared_ptr<State>> nextStates;
			for (auto &state : currentStates)
			{
				if (state->symbolTransitions.find(ch) != state->symbolTransitions.end())
				{
					auto nextState = state->symbolTransitions.at(ch);
					visitedStates.clear();
					addNextState(nextState, nextStates, visitedStates);
				}
			}
			currentStates = nextStates;
		}
		for (auto state : currentStates)
		{
			if (state->isEnd)
				return true;
		}
		return false;
	}

	void NfaParser::addNextState(const std::shared_ptr<State> currentState, std::vector<std::shared_ptr<State>>& nextStates, std::vector<std::shared_ptr<State>>& visitedStates)
	{
		if (!currentState->epsilonTransitions.empty())
		{
			for (auto nextState : currentState->epsilonTransitions)
			{
				if (std::find(visitedStates.begin(), visitedStates.end(), nextState) == visitedStates.end())
				{
					visitedStates.push_back(nextState);
					addNextState(nextState, nextStates, visitedStates);
				}
			}
		}
		else
		{
			nextStates.push_back(currentState);
		}
	}
}