#include "nfagenerator.h"
#include "parser.h"
#include <stack>

namespace Regex
{
	NfaGenerator::NfaGenerator(const std::string& infixExpression)
	{
		std::string postfixExpression = Parser::infixToPostfix(infixExpression);
		_nfa = *generateNfa(postfixExpression);
	}

	NfaGenerator::NFA NfaGenerator::getNfa()
	{
		return _nfa;
	}

	std::shared_ptr<NfaGenerator::State> NfaGenerator::createState(const bool isEnd)
	{
		auto state = std::make_shared<State>();
		state->isEnd = isEnd;
		return state;
	}

	std::unique_ptr<NfaGenerator::NFA> NfaGenerator::generateNfa(const std::string& postfixExpression)
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

	void NfaGenerator::addEpsilonTransition(const std::shared_ptr<State> from, const std::shared_ptr<State> to)
	{
		from->epsilonTransitions.push_back(to);
	}

	void NfaGenerator::addSymbolTransition(const std::shared_ptr<State> from, const char symbol, const std::shared_ptr<State> to)
	{
		from->symbolTransitions[symbol] = to;
	}

	std::unique_ptr<NfaGenerator::NFA> NfaGenerator::createEpsilonTransition()
	{
		std::shared_ptr<State> start = createState(false);
		std::shared_ptr<State> end = createState(true);
		addEpsilonTransition(start, end);
		return std::make_unique<NFA>(start,end);
	}

	std::unique_ptr<NfaGenerator::NFA> NfaGenerator::createSymbolTransition(const char symbol)
	{
		std::shared_ptr<State> start = createState(false);
		std::shared_ptr<State> end = createState(true);
		addSymbolTransition(start, symbol, end);
		return std::make_unique<NFA>(start, end);
	}

	std::unique_ptr<NfaGenerator::NFA> NfaGenerator::concatStates(const std::unique_ptr<NFA> first, const std::unique_ptr<NFA> second)
	{
		addEpsilonTransition(first->end, second->start);
		first->end->isEnd = false;
		return std::make_unique<NFA>(first->start,second->end);
	}

	std::unique_ptr<NfaGenerator::NFA> NfaGenerator::unionStates(const std::unique_ptr<NFA> first, const std::unique_ptr<NFA> second)
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

	std::unique_ptr<NfaGenerator::NFA> NfaGenerator::kleeneStarState(const std::unique_ptr<NFA> nfa)
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
}