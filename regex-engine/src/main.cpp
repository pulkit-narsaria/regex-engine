#include "regexmatcher.h"
#include <iostream>


int main(int argc, char* argv[])
{
	std::cout << "===== Started Regex Engine =====" << std::endl;
	std::cout << "Type \"Exit\" to exit at any given time." << std::endl;
	std::string pattern;
	std::cout << "Enter regex pattern : ";
	std::cin >> pattern;
	if (pattern == "Exit" || pattern == "exit")
		return 0;
	Regex::RegexMatcher matcher(pattern);
	std::string input;
	do
	{
		std::cout << "Enter text to match : ";
		std::cin >> input;
		if (input == "Exit" || input == "exit")
			break;
		auto result = matcher.search(input);
		std::cout << (result ? "Match found." : "No match found.") << std::endl;
	}
	while (input != "Exit" && input != "exit");
	
	std::cout << "===============================" << std::endl;
	return 0;
}