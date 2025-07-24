#include "parser.h"
#include <stack>

namespace Regex::Parser 
{
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

	std::string addConcatenations(const std::string& rawInfixExpression)
	{
		std::string modifiedExpression;
		for (size_t index = 0; index < rawInfixExpression.length(); index++)
		{
			auto currentChar = rawInfixExpression[index];
			modifiedExpression += currentChar;
			if (currentChar == '(' || currentChar == '|')
				continue;
			if (index < rawInfixExpression.length() - 1)
			{
				auto nextChar = rawInfixExpression[index + 1];
				if (isOperator(nextChar) || nextChar == ')')
				{
					continue;
				}
				modifiedExpression += '.';
			}
		}
		return modifiedExpression;
	}

	std::string infixToPostfix(const std::string& rawInfixExpression)
	{
		auto infixExpression = addConcatenations(rawInfixExpression);
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
}