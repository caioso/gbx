#include "Tokenizer.h"

using namespace std;

namespace gbxasm
{

void Tokenizer::ToToken(string inputString)
{
    _input = inputString;
    ExtactTokens();
}

void Tokenizer::ExtactTokens()
{
    stringstream lineStream(_input);
    auto currentLine = static_cast<string>("");
    auto line = 1ull;
    
    while(getline(lineStream, currentLine, '\n'))
    {
        istringstream stream(currentLine);
        auto word = static_cast<string>("");

        while (stream >> word)
        {
            Token token = 
            {
                .Token = word,
                .Line = line,
            };
            _tokens.push_back(token);
        }

        line++;
    }
}

vector<Token>& Tokenizer::Tokens()
{
    return _tokens;
}

}