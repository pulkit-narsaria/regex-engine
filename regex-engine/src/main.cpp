#include "regexmatcher.h"
#include <iostream>


int main(int argc, char* argv[])
{
	Regex::RegexMatcher matcher("ab*");
	auto result = matcher.search("abbbbc");
	std::cout << "Match result: " << (result ? "true" : "false") << std::endl;
	return 0;
}