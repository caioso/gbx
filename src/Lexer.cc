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
        else if (lexeme.first.compare(Lexemes::KeywordFUNC) == 0)
            token.Type = TokenType::KeywordFUNC;
        else if (lexeme.first.compare(Lexemes::KeywordEND) == 0)
            token.Type = TokenType::KeywordEND;
        else if (lexeme.first.compare(Lexemes::KeywordDECL) == 0)
            token.Type = TokenType::KeywordDECL;
        else if (lexeme.first.compare(Lexemes::KeywordBOOL) == 0)
            token.Type = TokenType::KeywordBOOL;
        else if (lexeme.first.compare(Lexemes::KeywordCHAR) == 0)
            token.Type = TokenType::KeywordCHAR;
        else if (lexeme.first.compare(Lexemes::KeywordBYTE) == 0)
            token.Type = TokenType::KeywordBYTE;
        else if (lexeme.first.compare(Lexemes::KeywordWORD) == 0)
            token.Type = TokenType::KeywordWORD;
        else if (lexeme.first.compare(Lexemes::KeywordDWORD) == 0)
            token.Type = TokenType::KeywordDWORD;
        else if (lexeme.first.compare(Lexemes::KeywordSTR) == 0)
            token.Type = TokenType::KeywordSTR;
        else if (lexeme.first.compare(Lexemes::KeywordAS) == 0)
            token.Type = TokenType::KeywordAS;
        else if (lexeme.first.compare(Lexemes::KeywordCONST) == 0)
            token.Type = TokenType::KeywordCONST;
        else if (lexeme.first.compare(Lexemes::KeywordFREE) == 0)
            token.Type = TokenType::KeywordFREE;
        else if (lexeme.first.compare(Lexemes::KeywordIF) == 0)
            token.Type = TokenType::KeywordIF;
        else if (lexeme.first.compare(Lexemes::KeywordTHEN) == 0)
            token.Type = TokenType::KeywordTHEN;
        else if (lexeme.first.compare(Lexemes::KeywordELSE) == 0)
            token.Type = TokenType::KeywordELSE;
        else if (lexeme.first.compare(Lexemes::KeywordWITH) == 0)
            token.Type = TokenType::KeywordWITH;
        else if (lexeme.first.compare(Lexemes::KeywordREPT) == 0)
            token.Type = TokenType::KeywordREPT;
        else if (lexeme.first.compare(Lexemes::KeywordTIMES) == 0)
            token.Type = TokenType::KeywordTIMES;
        else if (lexeme.first.compare(Lexemes::KeywordNEXT) == 0)
            token.Type = TokenType::KeywordNEXT;
        else if (lexeme.first.compare(Lexemes::KeywordEXIT) == 0)
            token.Type = TokenType::KeywordEXIT;
        else if (lexeme.first.compare(Lexemes::KeywordWHEN) == 0)
            token.Type = TokenType::KeywordWHEN;
        else if (lexeme.first.compare(Lexemes::KeywordIS) == 0)
            token.Type = TokenType::KeywordIS;
        else if (lexeme.first.compare(Lexemes::KeywordWHILE) == 0)
            token.Type = TokenType::KeywordWHILE;
        else if (lexeme.first.compare(Lexemes::KeywordALIAS) == 0)
            token.Type = TokenType::KeywordALIAS;
        else if (lexeme.first.compare(Lexemes::KeywordTRY) == 0)
            token.Type = TokenType::KeywordTRY;
        else if (lexeme.first.compare(Lexemes::KeywordCATCH) == 0)
            token.Type = TokenType::KeywordCATCH;
        else if (lexeme.first.compare(Lexemes::KeywordABORT) == 0)
            token.Type = TokenType::KeywordABORT;
        else if (lexeme.first.compare(Lexemes::KeywordTEST) == 0)
            token.Type = TokenType::KeywordTEST;
        else if (lexeme.first.compare(Lexemes::KeywordMACRO) == 0)
            token.Type = TokenType::KeywordMACRO;
        else if (lexeme.first.compare(Lexemes::KeywordMOVE) == 0)
            token.Type = TokenType::KeywordMOVE;
        else if (lexeme.first.compare(Lexemes::KeywordHIGH) == 0)
            token.Type = TokenType::KeywordHIGH;
        else if (lexeme.first.compare(Lexemes::KeywordLOW) == 0)
            token.Type = TokenType::KeywordLOW;
        // Operators
        else if (lexeme.first.compare(Lexemes::OperatorASSIGNMENT) == 0)
            token.Type = TokenType::OperatorASSIGNMENT;
        else if (lexeme.first.compare(Lexemes::OperatorEQUAL) == 0)
            token.Type = TokenType::OperatorEQUAL;
        else if (lexeme.first.compare(Lexemes::OperatorPLUS) == 0)
            token.Type = TokenType::OperatorPLUS;
        else if (lexeme.first.compare(Lexemes::OperatorTRHEEWAYCOMPARISON) == 0)
            token.Type = TokenType::OperatorTHREEWAYCOMPARISON;
        else if (lexeme.first.compare(Lexemes::OperatorMINUS) == 0)
            token.Type = TokenType::OperatorMINUS;
        else if (lexeme.first.compare(Lexemes::OperatorMULTIPLICATION) == 0)
            token.Type = TokenType::OperatorMULTIPLICATION;
        else if (lexeme.first.compare(Lexemes::OperatorBITWISEAND) == 0)
            token.Type = TokenType::OperatorBITWISEAND;
        else if (lexeme.first.compare(Lexemes::OperatorBITWISEOR) == 0)
            token.Type = TokenType::OperatorBITWISEOR;
        else if (lexeme.first.compare(Lexemes::OperatorBITWISENOT) == 0)
            token.Type = TokenType::OperatorBITWISENOT;
        else if (lexeme.first.compare(Lexemes::OperatorBITWISEXOR) == 0)
            token.Type = TokenType::OperatorBITWISEXOR;
        else if (lexeme.first.compare(Lexemes::OperatorLEFTSHIFT) == 0)
            token.Type = TokenType::OperatorLEFTSHIFT;
        else if (lexeme.first.compare(Lexemes::OperatorRIGHTSHIFT) == 0)
            token.Type = TokenType::OperatorRIGHTSHIFT;
        else if (lexeme.first.compare(Lexemes::OperatorDIFFERENT) == 0)
            token.Type = TokenType::OperatorDIFFERENT;
        else if (lexeme.first.compare(Lexemes::OperatorLOGICAND) == 0)
            token.Type = TokenType::OperatorLOGICAND;
        else if (lexeme.first.compare(Lexemes::OperatorLOGICOR) == 0)
            token.Type = TokenType::OperatorLOGICOR;
        else if (lexeme.first.compare(Lexemes::OperatorLESSTHAN) == 0)
            token.Type = TokenType::OperatorLESSTHAN;
        else if (lexeme.first.compare(Lexemes::OperatorGREATERTHAN) == 0)
            token.Type = TokenType::OperatorGREATERTHAN;
        else if (lexeme.first.compare(Lexemes::OperatorLESSTHANOREQUALTO) == 0)
            token.Type = TokenType::OperatorLESSTHANOREQUALTO;
        else if (lexeme.first.compare(Lexemes::OperatorGREATERTHANOREQUALTO) == 0)
            token.Type = TokenType::OperatorGREATERTHANOREQUALTO;
        else if (lexeme.first.compare(Lexemes::OperatorLOGICNOT) == 0)
            token.Type = TokenType::OperatorLOGICNOT;
        else if (lexeme.first.compare(Lexemes::OperatorAT) == 0)
            token.Type = TokenType::OperatorAT;
        else if (lexeme.first.compare(Lexemes::OperatorSEMICOLON) == 0)
            token.Type = TokenType::OperatorSEMICOLON;
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
    if (candidate[position] == '+' || candidate[position] == '=' || candidate[position] == '<' || candidate[position] == '>' ||
        candidate[position] == '/' || candidate[position] == '*' || candidate[position] == '&' || candidate[position] == '|' ||
        candidate[position] == '!' || candidate[position] == '~' || candidate[position] == '^' || candidate[position] == '-' ||
        candidate[position] == '@' || candidate[position] == ':')
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