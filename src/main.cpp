#include <iostream>
#include <fstream>
#include <string>
#include "ferry.hpp"

int main(int argc, char *argv[])
{
	std::string sourceCode;
	std::ifstream sourceFile;
	sourceFile.open(argv[1]);

	if (sourceFile.is_open())
	{
		std::string line;
		
		while (getline(sourceFile, line))
		{
			sourceCode += line + "\n";
		}
	}

	sourceFile.close();

	auto tokens = tokenize(sourceCode, {"(", ")", "[", "]", "<", ">", "{", "}", ";", "::", ":", "=", "*", "+", "-" "/"}, {"int", "string", "return"}, {"int", "float", "bool", "string", "null"});

	for (auto i: tokens)
		std::cout << i.tokenType << " " << i.literal << "\n";

	return 1;
}
