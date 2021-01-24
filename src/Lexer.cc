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

        // Keywords        
        if (lexeme.first.compare(Lexemes::KeywordPACK) == 0)
            token.Type = TokenType::KeywordPACK;
        // Operators
        else if (lexeme.first.compare(Lexemes::OperatorASSIGNMENT) == 0)
            token.Type = TokenType::OperatorASSIGNMENT;
        else if (lexeme.first.compare(Lexemes::OperatorEQUAL) == 0)
            token.Type = TokenType::OperatorEQUAL;
        else if (lexeme.first.compare(Lexemes::OperatorPLUS) == 0)
            token.Type = TokenType::OperatorPLUS;
        else if (lexeme.first.compare(Lexemes::OperatorTRHEEWAYCOMPARISON) == 0)
            token.Type = TokenType::OperatorTHREEWAYCOMPARISON;
        // Separators
        else if (lexeme.first.compare(Lexemes::SeparatorCOMMA) == 0)
            token.Type = TokenType::SeparatorCOMMA;
        else if (lexeme.first.compare(Lexemes::SeparatorOPENPARENTHESIS) == 0)
            token.Type = TokenType::SeparatorOPENPARENTHESIS;
        else if (lexeme.first.compare(Lexemes::SeparatorCLOSEPARENTHESIS) == 0)
            token.Type = TokenType::SeparatorCLOSEPARENTHESIS;
        else if (lexeme.first.compare(Lexemes::SeparatorOPENBRACKETS) == 0)
            token.Type = TokenType::SeparatorOPENBRACKETS;
        else if (lexeme.first.compare(Lexemes::SeparatorCLOSEBRACKETS) == 0)
            token.Type = TokenType::SeparatorCLOSEBRACKETS;
        else if (lexeme.first.compare(Lexemes::SeparatorOPENCURLYBRACKETS) == 0)
            token.Type = TokenType::SeparatorOPENCURLYBRACKETS;
        else if (lexeme.first.compare(Lexemes::SeparatorCLOSECURLYBRACKETS) == 0)
            token.Type = TokenType::SeparatorCLOSECURLYBRACKETS;
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
    auto columnCounter = static_cast<size_t>(0);

    for (auto i = size_t(0); i < lexeme.size(); ++i)
    {
        if (IsSeparatorOrOperator(lexeme, i))
        {
            if (accumulator.size() != 0)
                SaveSubLexeme(accumulator, column, subLexemes, columnCounter);
          
            SaveSubLexeme(ExtractOperatorOrSeparator(lexeme, i), column, subLexemes, columnCounter);
            CorrectLoopIndex(subLexemes, i);
            accumulator = "";
        }
        else
            accumulator += lexeme[i];
        
    }

    if (accumulator.size() != 0)
        subLexemes.push_back(make_pair(accumulator, column + columnCounter));

    return subLexemes;
}

inline void Lexer::SaveSubLexeme(string token, size_t column, std::vector<std::pair<std::string, size_t> >& subLexemes, size_t& columnCounter)
{
    subLexemes.push_back(make_pair(token, column + columnCounter));
    columnCounter += token.size();
}

inline bool Lexer::IsSeparatorOrOperator(string lexeme, size_t position)
{
    return IsPossibleOperator(lexeme, position) || IsPossibleSeparator(lexeme, position);
}

inline void Lexer::CorrectLoopIndex( std::vector<std::pair<std::string, size_t> >& subLexemes, size_t& i)
{
    if (subLexemes[subLexemes.size() - 1].first.size() >= 2)
        i += (subLexemes[subLexemes.size() - 1].first.size() - 1);
}

inline string Lexer::ExtractOperatorOrSeparator(string candidate, size_t column)
{
    if (IsPossibleOperator(candidate, column))
        return ExtractOperator(candidate, column);
    else
        return ExtractSeparator(candidate, column);
}

inline string Lexer::ExtractOperator(string candidate, size_t column)
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

inline string Lexer::ExtractSeparator(string candidate, size_t column)
{
    // Important note: Separators are *ALWAYS* one character only. That's why they are not accumulated like for Operators.
    string accumulator = "";
    
    if(IsPossibleSeparator(candidate, column)) 
        accumulator += candidate[column];

    return accumulator;
}

bool Lexer::IsPossibleOperator(string candidate, size_t position)
{
    if (candidate[position] == '+' || candidate[position] == '=' || candidate[position] == '<' || candidate[position] == '>')
        return true;

    return false;
}

bool Lexer::IsPossibleSeparator(string candidate, size_t position)
{
    if (candidate[position] == '{' || candidate[position] == '}' || candidate[position] == '(' || candidate[position] == ')' ||
        candidate[position] == '[' || candidate[position] == ']' || candidate[position] == ':' || candidate[position] == '\"' ||
        candidate[position] == '\'' || candidate[position] == ',')
        return true;

    return false;
}

void Lexer::ClearTokens()
{
    _tokens.resize(0);
}

}