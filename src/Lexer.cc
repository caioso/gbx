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
            auto tokens = EvaluateLexeme(lexeme, column);
            
            for (auto token : tokens)
            {
                token.Line = line;
                _tokens.push_back(token);
            }
        }

        line++;
    }
}

vector<Token> Lexer::EvaluateLexeme(string lexeme, size_t column)
{
    auto subLexemes = FindSubLexemes(lexeme, column);
    vector<Token> tokens;

    for (auto lexeme : subLexemes)
    {
        Token token =
        {
            .Lexeme = lexeme.first,
            .Column = lexeme.second
        };
        
        if (lexeme.first.compare(Lexemes::LexemeKeywordPACK) == 0)
            token.Type = TokenType::KeywordPACK;
        else if (lexeme.first.compare(Lexemes::LexemeOperatorASSIGNMENT) == 0)
            token.Type = TokenType::OperatorASSIGNMENT;
        else if (lexeme.first.compare(Lexemes::LexemeOperatorEQUAL) == 0)
            token.Type = TokenType::OperatorEQUAL;
        else if (lexeme.first.compare(Lexemes::LexemeOperatorPLUS) == 0)
            token.Type = TokenType::OperatorPLUS;
        else if (lexeme.first.compare(Lexemes::LexemeOperatorTRHEEWAYCOMPARISON) == 0)
            token.Type = TokenType::OperatorTHREEWAYCOMPARISON;
        else
            token.Type = TokenType::UnknownToken;

        tokens.push_back(token);
    }

    return tokens;
}

vector<pair<string, size_t> > Lexer::FindSubLexemes(string lexeme, size_t column)
{
    vector<pair<string, size_t> > subLexemes;
    string accumulator = "";
    auto columnCounter = 0;

    for (auto i = size_t(0); i < lexeme.size(); ++i)
    {
        if (IsPossibleOperator(lexeme, i))
        {
            // If the token starts with an operator, the accumulator will be empty
            if (accumulator.size() != 0)
            {
                subLexemes.push_back(make_pair(accumulator, column + columnCounter));
                columnCounter += accumulator.size();
            }

            auto operatorOrSeparator = ExtractOperator(lexeme, i);
            subLexemes.push_back(make_pair(operatorOrSeparator, column + columnCounter));
            columnCounter += operatorOrSeparator.size();

            if (operatorOrSeparator.size() >= 2)
                i += (operatorOrSeparator.size() - 1);

            accumulator = "";
        }
        else
        {
            accumulator += lexeme[i];
        }
        
    }

    if (accumulator.size() != 0)
        subLexemes.push_back(make_pair(accumulator, column + columnCounter));

    return subLexemes;
}

std::string Lexer::ExtractOperator(std::string candidate, size_t column)
{
    string accumulator = "";
    
    for (auto i = column; i < candidate.size(); ++i)
    {
        accumulator += candidate[i];
        if(!IsPossibleOperator(candidate, i)) 
        {
            accumulator = accumulator.substr(0, accumulator.size() - 1);
            break;
        }
    }            

    return accumulator;
}

bool Lexer::IsPossibleOperator(std::string candidate, size_t position)
{
    if (candidate[position] == '+' || candidate[position] == '=' || candidate[position] == '<' || candidate[position] == '>')
        return true;

    return false;
}

void Lexer::ClearTokens()
{
    _tokens.resize(0);
}

}