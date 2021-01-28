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

vector<Token> Lexer::EvaluateLexeme(string originalLexeme, size_t column)
{
    auto subLexemes = FindSubLexemes(originalLexeme, column);
    vector<Token> tokens;

    for (auto lexeme : subLexemes)
    {
        Token token =
        {
            .Lexeme = lexeme.first,
            .Column = lexeme.second
        };

        // Numeric Literals
        if (IsNumericLiteral(lexeme.first))
            token.Type = IdentifyNumericLiteral(lexeme.first);

        // Keywords        
        else if (lexeme.first.compare(Lexemes::KeywordPACK) == 0)
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
        else if (lexeme.first.compare(Lexemes::KeywordBIT) == 0)
            token.Type = TokenType::KeywordBIT;
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
        else if (lexeme.first.compare(Lexemes::OperatorDOT) == 0)
            token.Type = TokenType::OperatorDOT;
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

inline bool Lexer::IsNumericLiteral(string_view lexeme)
{
    if (!IsInitialDigit(lexeme, 0))
        return false;

    for (auto i = static_cast<size_t>(1); i < lexeme.size(); i++)
        if (!IsDigit(lexeme, i))
        {
            stringstream ss;
            ss << "Invalid character '" << lexeme[i] << "' near numeric literal '" << lexeme <<"'";
            throw LexerException(ss.str());
        }

    return true;
}

inline TokenType Lexer::IdentifyNumericLiteral(string_view candidate)
{
    if(candidate[0] == '0')
    {
        auto prefix = candidate.substr(0, 2);
        
        if (prefix.compare(Lexemes::LiteralNumericHEXADECIMALBASE) == 0)
            // Invalid hexadecimal values will be caught when evaluating the token's charactets
            return TokenType::LiteralNumericHEXADECIMAL;
        else if (prefix.compare(Lexemes::LiteralNumericDECIMALBASE) == 0)
        {
            ValidateDecimalLiteral(candidate.substr(2, candidate.size() - 2));
            return TokenType::LiteralNumericDECIMAL;
        }
        else if (prefix.compare(Lexemes::LiteralNumericOCTALBASE) == 0)
        {
            ValidateOctalLiteral(candidate.substr(2, candidate.size() - 2));
            return TokenType::LiteralNumericOCTAL;
        }
        else if (prefix.compare(Lexemes::LiteralNumericBINARYBASE) == 0)
        {
            ValidateBinaryLiteral(candidate.substr(2, candidate.size() - 2));
            return TokenType::LiteralNumericBINARY;
        }
        else
        {
            ValidateDecimalLiteral(candidate);
            return TokenType::LiteralNumericDECIMAL;
        }
    }
    else
    {
        ValidateDecimalLiteral(candidate);
        return TokenType::LiteralNumericDECIMAL;
    }
}

inline void Lexer::ValidateDecimalLiteral(string_view candidate)
{
    for (auto character : candidate)
        if (character != '1' && character != '2' && character != '3' && character != '4' &&
            character != '5' && character != '6' && character != '7' && character != '8' &&
            character != '9' && character != '0')
        {
            stringstream ss;
            ss << "Invalid decimal numeric literal '" << candidate << "'";
            throw LexerException(ss.str());
        }
}

inline void Lexer::ValidateOctalLiteral(string_view candidate)
{
    for (auto character : candidate)
        if (character != '1' && character != '2' && character != '3' && character != '4' &&
            character != '5' && character != '6' && character != '7' && character != '0')
        {
            stringstream ss;
            ss << "Invalid octal numeric literal '" << candidate << "'";
            throw LexerException(ss.str());
        }
}

inline void Lexer::ValidateBinaryLiteral(string_view candidate)
{
    for (auto character : candidate)
        if (character != '1' && character != '0')
        {
            stringstream ss;
            ss << "Invalid binary numeric literal '" << candidate << "'";
            throw LexerException(ss.str());
        }
}

inline bool Lexer::IsDigit(string_view candidate, size_t position)
{
    if (candidate[position] == '1' || candidate[position] == '2' || candidate[position] == '3' || candidate[position] == '4' ||
        candidate[position] == '5' || candidate[position] == '6' || candidate[position] == '7' || candidate[position] == '8' ||
        candidate[position] == '9' || candidate[position] == '0' || candidate[position] == 'A' || candidate[position] == 'B' ||
        candidate[position] == 'C' || candidate[position] == 'D' || candidate[position] == 'E' || candidate[position] == 'F' ||
        candidate[position] == 'a' || candidate[position] == 'b' || candidate[position] == 'c' || candidate[position] == 'd' || 
        candidate[position] == 'e' || candidate[position] == 'f' || candidate[position] == 'X' || candidate[position] == 'x' ||
        candidate[position] == 'o' || candidate[position] == 'O')
        return true;

    return false;
}

inline bool Lexer::IsInitialDigit(string_view candidate, size_t position)
{
    if (candidate[position] == '1' || candidate[position] == '2' || candidate[position] == '3' || candidate[position] == '4' ||
        candidate[position] == '5' || candidate[position] == '6' || candidate[position] == '7' || candidate[position] == '8' ||
        candidate[position] == '9' || candidate[position] == '0')
        return true;

    return false;
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

inline bool Lexer::IsSeparatorOrOperator(string_view lexeme, size_t position)
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

bool Lexer::IsPossibleOperator(string_view candidate, size_t position)
{
    if (candidate[position] == '+' || candidate[position] == '=' || candidate[position] == '<' || candidate[position] == '>' ||
        candidate[position] == '/' || candidate[position] == '*' || candidate[position] == '&' || candidate[position] == '|' ||
        candidate[position] == '!' || candidate[position] == '~' || candidate[position] == '^' || candidate[position] == '-' ||
        candidate[position] == '@' || candidate[position] == ':' || candidate[position] == '.')
        return true;

    return false;
}

bool Lexer::IsPossibleSeparator(string_view candidate, size_t position)
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