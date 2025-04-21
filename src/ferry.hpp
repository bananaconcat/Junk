#include <string>
#include <vector>
#include <iostream>

const int MAX_LEN = 256;

enum FTokenType
{
	TOKEN_SYMBOL,
	TOKEN_IDENT,
	TOKEN_KEYWORD,
	TOKEN_D_TYPE,
	TOKEN_STRING
};

class FToken
{
	public:
		FTokenType tokenType;
		std::string literal;
};

int customDebugCount = 0;

void customDebug()
{
	customDebugCount++;
	std::cout << customDebugCount;
}

std::vector<FToken> tokenize(std::string sourceCode, const std::vector<std::string> symbols, const std::vector<std::string> keywords, const std::vector<std::string> dataTypes)
{
	std::vector<FToken> tokens;
	int currentIndex = 0;
	int currentLine = 1;
	int currentColumn = 0;
	int startIndex = 0;

	while (currentIndex < sourceCode.length())
	{
		bool matchedSymbol = false;
		currentColumn++;

		for (int i = 0; i < symbols.size(); i++)
		{
			std::string temps = sourceCode.substr(currentIndex, symbols[i].size());

			if (temps == symbols[i])
			{
				tokens.push_back({TOKEN_SYMBOL, symbols[i]});
				currentIndex += symbols[i].size();
				matchedSymbol = true;
				break;
			}
		}

		if (matchedSymbol)
			continue;

		if (sourceCode[currentIndex] == '\"')
		{
			startIndex = currentIndex;
			currentIndex++;

			while (currentIndex < sourceCode.size() && sourceCode[currentIndex] != '\"' && currentIndex - startIndex < MAX_LEN)
			{
				currentIndex++;
				currentColumn++;
			}

			currentIndex++;
			
			tokens.push_back({TOKEN_STRING, sourceCode.substr(startIndex, currentIndex - startIndex)});

			continue;
		}

		if (isalpha(sourceCode[currentIndex]))
		{
			startIndex = currentIndex;

			while (currentIndex < sourceCode.size() && (isalpha(sourceCode[currentIndex]) || isdigit(sourceCode[currentIndex])) && currentIndex - startIndex < MAX_LEN)
			{
				currentIndex++;
				currentColumn++;
			}

			std::string word = sourceCode.substr(startIndex, currentIndex - startIndex);
			int wordType = 0; // 0 Identifier, 1 Keyword, 2 Data type

			for (const auto& keyword : keywords)
			{
				if (keyword == word)
				{
					wordType = 1;
					break;
				}
			}

			for (const auto& dType : dataTypes)
			{
				if (dType == word)
				{
					wordType = 2;
					break;
				}
			}

			if (wordType == 0)
			{
				tokens.push_back({TOKEN_IDENT, word});
			}
			else if (wordType == 1)
			{
				tokens.push_back({TOKEN_KEYWORD, word});
			}
			else if (wordType == 2)
			{
				tokens.push_back({TOKEN_D_TYPE, word});
			}

			continue;
		}

		currentIndex++;
	}

	return tokens;
};

std::string cg_collect_variables(std::vector<FToken> tokens)
{
	std::string assembly_output = "section .data\n";

	for (int i = 0; i < tokens.size(); i++)
	{
		if (tokens[i].tokenType == TOKEN_D_TYPE)
		{
			i++;
			if (tokens[i].tokenType == TOKEN_SYMBOL && tokens[i].literal == ":")
			{
				i++;
				if (tokens[i].tokenType == TOKEN_IDENT && (tokens[i + 1].literal == "=" || tokens[i + 1].literal == ";"))
				{
					std::string t_dt = tokens[i - 2].literal;

					if (t_dt == "string" || t_dt == "int8")
						t_dt = " db";
					else if (t_dt == "int16")
						t_dt = " dw";
					else if (t_dt == "int32")
						t_dt = " dd";
					else if (t_dt == "int64")
						t_dt = " dq";
					else
						t_dt = " db";

					assembly_output += tokens[i].literal + t_dt + " 0\n";
					i++;
				}
			}
		}
	}

	return assembly_output;
}

std::string cg_declaration(std::vector<FToken> tokens, int& pos)
{
	std::string assembly_output = "";

	if (tokens[pos].tokenType == TOKEN_D_TYPE)
	{
		pos++;
		if (tokens[pos].tokenType == TOKEN_SYMBOL && tokens[pos].literal == ":")
		{
			pos++;
			if (tokens[pos].tokenType == TOKEN_IDENT)
			{
				pos++;
				if (tokens[pos].tokenType == TOKEN_SYMBOL && tokens[pos].literal == "(")
				{
					// this assumes theres no parameters, which will be the case for testing right now.
					assembly_output += tokens[pos-1].literal + ":\n";
					pos++;
					pos++;
					
					for (pos < tokens.size(); tokens[pos].literal != "{";)
					{
						pos++;
					}

					if (pos < tokens.size())
						pos++;
						
				}
				else if (tokens[pos].tokenType == TOKEN_SYMBOL && tokens[pos].literal == "=")
				{

				}
			}
		}
	}

	return assembly_output;
}

void cg_generate(std::vector<FToken> tokens)
{

	std::string assembly_output = cg_collect_variables(tokens) + "section .text\n.global main\n";
	int position = 0;

	while (position < tokens.size())
	{
		assembly_output += cg_declaration(tokens, position);

		position++;

		if (position == tokens.size()) break;
	}

	std::cout << assembly_output;
}
