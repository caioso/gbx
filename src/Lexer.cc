#include "Lexer.h"

using namespace std;

namespace gbxasm
{

Lexer::Lexer()
{}

vector<Token>& Lexer::Tokens()
{
    return _tokens;
}

void Lexer::Tokenize(string_view input)
{
    ClearTokens();
    ExtractTokens(input);
}

void Lexer::ExtractTokens(string_view input)
{
    stringstream lineStream(input.data());
    auto currentLine = static_cast<string>("");
    auto line = static_cast<size_t>(1);

    while(getline(lineStream, currentLine, '\n'))
    {
        stringstream stream(currentLine);
        auto lexeme = static_cast<string>("");
        
        while (stream >> lexeme)
        {
            auto column = currentLine.find(lexeme) + 1;
            auto token = EvaluateLexeme(lexeme);
            
            token.Line = line;
            token.Column = column;
            _tokens.push_back(token);
        }

        line++;
    }
}

Token Lexer::EvaluateLexeme(string lexeme)
{
    Token token;
    token.Lexeme = lexeme;
    if (lexeme.compare(Lexemes::KeywordPack) == 0)
        token.Type = TokenType::KeywordPACK;
    
    return token;
}

vector<string> Lexer::TrySubdivideLexeme(string lexeme)
{
    vector<string> subLexemes;
    string accumulator = "";

    for (auto i = size_t(0); i < lexeme.size(); ++i)
    {
        accumulator += lexeme[i];
        if (Lexemes::OperatorPlus.compare(lexeme.substr(i, 1)) == 0)
        {
        }
    }

    return subLexemes;
}

void Lexer::ClearTokens()
{
    _tokens.resize(0);
}

}