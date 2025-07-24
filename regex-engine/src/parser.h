#pragma once

#include <string>

namespace Regex::Parser 
{
	std::string addConcatenations(const std::string& rawInfixExpression);
	std::string infixToPostfix(const std::string& rawInfixExpression);
}