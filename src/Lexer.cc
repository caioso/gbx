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
    auto globalCounter = 0;

    _stringLiteralAccumulationStarted = false;
    _stringLiteralAccumulationEnded = false;    

    while(getline(lineStream, currentLine, '\n'))
    {
        shared_ptr<stringstream> stream = make_shared<stringstream>(currentLine);
        auto lexeme = static_cast<string>("");
        auto column = 0;

        while ((*stream) >> lexeme)
        {
            column = currentLine.find(lexeme, column);  
            auto tokens = EvaluateLexeme(lexeme, column + 1, globalCounter);
            
            for (auto token : tokens)
            {
                token.Line = line;
                _tokens.push_back(token);
            }

            ExtractAllStringTokenIfNeeded(input);
        }

        globalCounter += currentLine.size() + 1;
        line++;
    }

    if (_stringLiteralAccumulationStarted || _stringLiteralAccumulationEnded)
        throw LexerException("Non-terminated string literal found");
}

vector<Token> Lexer::EvaluateLexeme(string originalLexeme, size_t column, size_t globalCounter)
{
    auto subLexemes = FindSubLexemes(originalLexeme, column);
    vector<Token> tokens;

    for (auto lexeme : subLexemes)
    {
        Token token =
        {
            .Lexeme = lexeme.first,
            .Column = lexeme.second,
            .GlobalPosition = (lexeme.second - 1) + globalCounter,
        };

        // Numeric Literals
        if (IsNumericLiteral(lexeme.first))
            token.Type = IdentifyNumericLiteral(lexeme.first);

        // String Literals
        else if (IsStringLiteral(lexeme.first))
            token.Type = TokenType::SeparatorDOUBLEQUOTES;
        
        // String Literals
        else if (IsCharLiteral(lexeme.first))
            token.Type = TokenType::LiteralCHAR;

        // Boolean Literals
        else if (lexeme.first.compare(Lexemes::LiteralBooleanTRUE) == 0)
            token.Type = TokenType::LiteralBooleanTRUE;
        else if (lexeme.first.compare(Lexemes::LiteralBooleanFALSE) == 0)
            token.Type = TokenType::LiteralBooleanFALSE;

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
        else if (lexeme.first.compare(Lexemes::KeywordCHECK) == 0)
            token.Type = TokenType::KeywordCHECK;
        else if (lexeme.first.compare(Lexemes::KeywordASSRT) == 0)
            token.Type = TokenType::KeywordASSRT;
        else if (lexeme.first.compare(Lexemes::KeywordPASS) == 0)
            token.Type = TokenType::KeywordPASS;
        else if (lexeme.first.compare(Lexemes::KeywordFAIL) == 0)
            token.Type = TokenType::KeywordFAIL;
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

        // Instruction Mnemonics
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicNOP) == 0)
            token.Type = TokenType::InstructionMnemonicNOP;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicLD) == 0)
            token.Type = TokenType::InstructionMnemonicLD;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicINC) == 0)
            token.Type = TokenType::InstructionMnemonicINC;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicDEC) == 0)
            token.Type = TokenType::InstructionMnemonicDEC;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicRLCA) == 0)
            token.Type = TokenType::InstructionMnemonicRLCA;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicADD) == 0)
            token.Type = TokenType::InstructionMnemonicADD;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicRRCA) == 0)
            token.Type = TokenType::InstructionMnemonicRRCA;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicSTOP) == 0)
            token.Type = TokenType::InstructionMnemonicSTOP;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicRLA) == 0)
            token.Type = TokenType::InstructionMnemonicRLA;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicJR) == 0)
            token.Type = TokenType::InstructionMnemonicJR;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicRRA) == 0)
            token.Type = TokenType::InstructionMnemonicRRA;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicDAA) == 0)
            token.Type = TokenType::InstructionMnemonicDAA;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicCPL) == 0)
            token.Type = TokenType::InstructionMnemonicCPL;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicSCF) == 0)
            token.Type = TokenType::InstructionMnemonicSCF;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicCCF) == 0)
            token.Type = TokenType::InstructionMnemonicCCF;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicHALT) == 0)
            token.Type = TokenType::InstructionMnemonicHALT;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicADC) == 0)
            token.Type = TokenType::InstructionMnemonicADC;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicSUB) == 0)
            token.Type = TokenType::InstructionMnemonicSUB;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicSBC) == 0)
            token.Type = TokenType::InstructionMnemonicSBC;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicAND) == 0)
            token.Type = TokenType::InstructionMnemonicAND;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicXOR) == 0)
            token.Type = TokenType::InstructionMnemonicXOR;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicOR) == 0)
            token.Type = TokenType::InstructionMnemonicOR;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicCP) == 0)
            token.Type = TokenType::InstructionMnemonicCP;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicRET) == 0)
            token.Type = TokenType::InstructionMnemonicRET;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicPOP) == 0)
            token.Type = TokenType::InstructionMnemonicPOP;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicJP) == 0)
            token.Type = TokenType::InstructionMnemonicJP;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicPUSH) == 0)
            token.Type = TokenType::InstructionMnemonicPUSH;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicCALL) == 0)
            token.Type = TokenType::InstructionMnemonicCALL;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicRST) == 0)
            token.Type = TokenType::InstructionMnemonicRST;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicEI) == 0)
            token.Type = TokenType::InstructionMnemonicEI;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicDI) == 0)
            token.Type = TokenType::InstructionMnemonicDI;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicRLC) == 0)
            token.Type = TokenType::InstructionMnemonicRLC;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicRRC) == 0)
            token.Type = TokenType::InstructionMnemonicRRC;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicRL) == 0)
            token.Type = TokenType::InstructionMnemonicRL;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicRR) == 0)
            token.Type = TokenType::InstructionMnemonicRR;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicSLA) == 0)
            token.Type = TokenType::InstructionMnemonicSLA;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicSRA) == 0)
            token.Type = TokenType::InstructionMnemonicSRA;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicSWAP) == 0)
            token.Type = TokenType::InstructionMnemonicSWAP;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicSRL) == 0)
            token.Type = TokenType::InstructionMnemonicSRL;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicBIT) == 0)
            token.Type = TokenType::InstructionMnemonicBIT;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicRES) == 0)
            token.Type = TokenType::InstructionMnemonicRES;
        else if (lexeme.first.compare(Lexemes::InstructionMnemonicSET) == 0)
            token.Type = TokenType::InstructionMnemonicSET;
        else
            token.Type = TokenType::Identifier;

        tokens.push_back(token);
    }

    return tokens;
}

inline bool Lexer::IsStringLiteral (string_view lexeme)
{
    if (lexeme[0] == '\"')
        return true;
    return false;
}

inline bool Lexer::IsCharLiteral (string_view lexeme)
{
    if (lexeme[0] == '\'')
        return true;
    return false;
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
        if (IsSeparatorOrOperator(lexeme, i) || IsPossibleStringLiteralMarker(lexeme, i) || IsPossibleCharLiteralMarker(lexeme, i))
        {
            if (accumulator.size() != 0)
            {
                SaveSubLexeme(accumulator, column, subLexemes, columnCounter);
                accumulator = "";
            }
          
            SaveSubLexeme(ExtractOperatorSeparatorOrMarker(lexeme, i), column, subLexemes, columnCounter);
            CorrectLoopIndex(subLexemes, i);
            EvaluateStringLimits(lexeme, i);
        }
        else
            accumulator += lexeme[i];
        
    }

    if (accumulator.size() != 0)
        subLexemes.push_back(make_pair(accumulator, column + columnCounter));

    ConvertCharLiteral(subLexemes);    

    return subLexemes;
}

inline void Lexer::ConvertCharLiteral(std::vector<std::pair<std::string, size_t> >& subLexemes)
{
    for (auto i = static_cast<size_t>(0x00); i < subLexemes.size(); ++i)
        if (IsPossibleCharLiteralMarker(subLexemes[i].first, 0))
            subLexemes[i] = make_pair(EvaluateAndConvertChar(subLexemes[i].first), subLexemes[i].second);
}

inline void Lexer::EvaluateStringLimits(string lexeme, size_t column)
{
    if (IsPossibleStringLiteralMarker(lexeme, column))
    {
        if (!_stringLiteralAccumulationStarted)
            _stringLiteralAccumulationStarted = true;
        else
            _stringLiteralAccumulationEnded = true;
    }
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

inline string Lexer::ExtractOperatorSeparatorOrMarker(string candidate, size_t column)
{
    if (IsPossibleOperator(candidate, column))
        return ExtractOperator(candidate, column);
    else if (IsPossibleStringLiteralMarker(candidate, column))
        return ExtractStringLiteralMarker(candidate, column);
    else if (IsPossibleCharLiteralMarker(candidate, column))
        return ExtractCharLiteralFromCandidate(candidate, column);
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

inline string Lexer::ExtractStringLiteralMarker(string candidate, size_t column)
{
    // Important note: Separators are *ALWAYS* one character only. That's why they are not accumulated like for Operators.
    string accumulator = "";
    
    if(IsPossibleStringLiteralMarker(candidate, column)) 
        accumulator += candidate[column];

    return accumulator;
}

inline string Lexer::ExtractCharLiteralFromCandidate(string candidate, size_t column)
{
    // Important note: Separators are *ALWAYS* one character only. That's why they are not accumulated like for Operators.
    string accumulator = "";
    auto counter = static_cast<size_t>(0);
    auto extracted = ExtractPossibleSubCharLiteral(candidate, column);

    EvaluateCharLiteralSize(extracted);    
    accumulator += AccumulateFirstSeparatorOfCharLiteral(extracted, counter);
    accumulator += AccumulateContentOfCharLiteral(extracted, counter);
    accumulator += AccumulateSecondSeparatorOfCharLiteral(extracted, counter);

    return accumulator;
}

inline string Lexer::ExtractPossibleSubCharLiteral(string candidate, size_t column)
{
    auto substring = candidate.substr(column, candidate.size() - column);
    auto accumulator = string("");
    auto previous = static_cast<char>(0x00);
    auto started = false;
    for (auto c : substring)
    {
        accumulator += c;
        if (!started && c == '\'')
            started = true;
        else if (started && c == '\'')
        {
            if (previous != '\\')
                return accumulator;
        }

        previous = c;
    }   

    return accumulator;
}

inline string Lexer::AccumulateFirstSeparatorOfCharLiteral(string candidate, size_t& column)
{
    auto accumulator = string("");
    if(IsPossibleCharLiteralMarker(candidate, column)) 
        accumulator += candidate[column++];

    return accumulator;
}

inline string Lexer::AccumulateContentOfCharLiteral(string candidate, size_t& column)
{
    auto accumulator = string("");
    if (candidate.size() == 0x04)
    {
        accumulator += candidate[column++];
        accumulator += candidate[column++];
    }
    else
        accumulator += candidate[column++];

    return accumulator;
}   
inline string Lexer::AccumulateSecondSeparatorOfCharLiteral(string candidate, size_t& column)
{
    auto accumulator = string("");
    if(IsPossibleCharLiteralMarker(candidate, column)) 
        accumulator += candidate[column++];
    else
        throw LexerException("char literals must be one or two characters long");

    return accumulator;
}

inline void Lexer::EvaluateCharLiteralSize(string candidate)
{
    if (candidate.size() != 3 && candidate.size() != 4)
    {
        if (candidate.size() == 2 || candidate.size() == 1)
            throw LexerException("Non-terminated char literal found");
        else
        {
            stringstream ss;
            ss << "invalid char literal found (" << candidate << ")";
            throw LexerException(ss.str());    
        }
    }
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
        candidate[position] == '[' || candidate[position] == ']' || candidate[position] == ':' || candidate[position] == ',')
        return true;

    return false;
}

bool Lexer::IsPossibleStringLiteralMarker(string_view candidate, size_t position)
{
    if (candidate[position] == '\"')
        return true;

    return false;
}

bool Lexer::IsPossibleCharLiteralMarker(string_view candidate, size_t position)
{
    if (candidate[position] == '\'')
        return true;

    return false;
}


inline void Lexer::ExtractAllStringTokenIfNeeded(string_view input)
{
    if (_stringLiteralAccumulationStarted && _stringLiteralAccumulationEnded)
    {
        while(HasUnmergedStrings())
        {
            auto endIndex = FindTokenByType(TokenType::SeparatorDOUBLEQUOTES, _tokens.size() - 1);
            auto startIndex = FindTokenByType(TokenType::SeparatorDOUBLEQUOTES, endIndex - 1);
            auto stringToken = GenerateStringToken(startIndex, endIndex, input);

            _tokens.insert(begin(_tokens) + startIndex, stringToken);
        }

        ClearStringLimitFlags();
    }
}

inline bool Lexer::HasUnmergedStrings()
{
    for (auto token : _tokens)
        if (token.Type == TokenType::SeparatorDOUBLEQUOTES)
            return true;

    return false;
}

inline Token Lexer::GenerateStringToken(size_t startIndex, size_t endIndex, string_view input)
{
    auto stringLiteral = string(input.substr(_tokens[startIndex].GlobalPosition, (_tokens[endIndex].GlobalPosition - _tokens[startIndex].GlobalPosition + 1)));
    auto startLine = _tokens[startIndex].Line;
    auto startColumn = _tokens[startIndex].Column;
    auto startGlobalPosition = _tokens[startIndex].GlobalPosition;
    
    _tokens.erase(begin(_tokens) + startIndex, begin(_tokens) + endIndex + 1);
    
    Token stringLiteralToken = 
    {
        .Lexeme = stringLiteral,
        .Line = startLine,
        .Column = startColumn,
        .Type = TokenType::LiteralSTRING,
        .GlobalPosition = startGlobalPosition
    };

    return stringLiteralToken;
}

inline void Lexer::ClearStringLimitFlags()
{
    _stringLiteralAccumulationStarted = false;
    _stringLiteralAccumulationEnded = false;
}

inline size_t Lexer::FindTokenByType(TokenType type, size_t startIndex)
{
    for (auto i = startIndex; i >= 0; --i)
        if (_tokens[i].Type == type)
            return i;

    return numeric_limits<size_t>().max();
}

inline std::string Lexer::EvaluateAndConvertChar(std::string_view originalChar)
{
    if (originalChar.compare("'\\s'") == 0)
        return "' '";
    else if (originalChar.compare("'!'") == 0)
        return "'!'";
    else if (originalChar.compare("'\\\"'") == 0)
        return "'\"'";
    else if (originalChar.compare("'#'") == 0)
        return "'#'";
    else if (originalChar.compare("'$'") == 0)
        return "'$'";
    else if (originalChar.compare("'%'") == 0)
        return "'%'";
    else if (originalChar.compare("'&'") == 0)
        return "'&'";
    else if (originalChar.compare("'\\''") == 0)
        return "'''";
    else if (originalChar.compare("'('") == 0)
        return "'('";
    else if (originalChar.compare("')'") == 0)
        return "')'";
    else if (originalChar.compare("'*'") == 0)
        return "'*'";
    else if (originalChar.compare("'+'") == 0)
        return "'+'";
    else if (originalChar.compare("','") == 0)
        return "','";
    else if (originalChar.compare("'-'") == 0)
        return "'-'";
    else if (originalChar.compare("'.'") == 0)
        return "'.'";
    else if (originalChar.compare("'/'") == 0)
        return "'/'"; 
    else if (originalChar.compare("'0'") == 0)
        return "'0'";
    else if (originalChar.compare("'1'") == 0)
        return "'1'";
    else if (originalChar.compare("'2'") == 0)
        return "'2'";
    else if (originalChar.compare("'3'") == 0)
        return "'3'";
    else if (originalChar.compare("'4'") == 0)
        return "'4'";
    else if (originalChar.compare("'5'") == 0)
        return "'5'";
    else if (originalChar.compare("'6'") == 0)
        return "'6'";
    else if (originalChar.compare("'7'") == 0)
        return "'7'";
    else if (originalChar.compare("'8'") == 0)
        return "'8'";
    else if (originalChar.compare("'9'") == 0)
        return "'9'";
    else if (originalChar.compare("':'") == 0)
        return "':'";
    else if (originalChar.compare("';'") == 0)
        return "';'";
    else if (originalChar.compare("'<'") == 0)
        return "'<'";
    else if (originalChar.compare("'='") == 0)
        return "'='";
    else if (originalChar.compare("'>'") == 0)
        return "'>'";
    else if (originalChar.compare("'?'") == 0)
        return "'?'";
    else if (originalChar.compare("'@'") == 0)
        return "'@'";
    else if (originalChar.compare("'A'") == 0)
        return "'A'";
    else if (originalChar.compare("'B'") == 0)
        return "'B'";
    else if (originalChar.compare("'C'") == 0)
        return "'C'";
    else if (originalChar.compare("'D'") == 0)
        return "'D'";
    else if (originalChar.compare("'E'") == 0)
        return "'E'";
    else if (originalChar.compare("'F'") == 0)
        return "'F'";
    else if (originalChar.compare("'G'") == 0)
        return "'G'";
    else if (originalChar.compare("'H'") == 0)
        return "'H'";
    else if (originalChar.compare("'I'") == 0)
        return "'I'";
    else if (originalChar.compare("'J'") == 0)
        return "'J'";
    else if (originalChar.compare("'K'") == 0)
        return "'K'";
    else if (originalChar.compare("'L'") == 0)
        return "'L'";
    else if (originalChar.compare("'M'") == 0)
        return "'M'";
    else if (originalChar.compare("'N'") == 0)
        return "'N'";
    else if (originalChar.compare("'O'") == 0)
        return "'O'";
    else if (originalChar.compare("'P'") == 0)
        return "'P'";
    else if (originalChar.compare("'Q'") == 0)
        return "'Q'";
    else if (originalChar.compare("'R'") == 0)
        return "'R'";
    else if (originalChar.compare("'S'") == 0)
        return "'S'";
    else if (originalChar.compare("'T'") == 0)
        return "'T'";
    else if (originalChar.compare("'U'") == 0)
        return "'U'";
    else if (originalChar.compare("'V'") == 0)
        return "'V'";
    else if (originalChar.compare("'W'") == 0)
        return "'W'";
    else if (originalChar.compare("'X'") == 0)
        return "'X'";
    else if (originalChar.compare("'Y'") == 0)
        return "'Y'";
    else if (originalChar.compare("'Z'") == 0)
        return "'Z'";
    else if (originalChar.compare("'['") == 0)
        return "'['";
    else if (originalChar.compare("']'") == 0)
        return "']'";
    else if (originalChar.compare("'\\'") == 0)
        return "'\\'";
    else if (originalChar.compare("'^'") == 0)
        return "'^'";
    else if (originalChar.compare("'_'") == 0)
        return "'_'";
    else if (originalChar.compare("'`'") == 0)
        return "'`'";
    else if (originalChar.compare("'a'") == 0)
        return "'a'";
    else if (originalChar.compare("'b'") == 0)
        return "'b'";
    else if (originalChar.compare("'c'") == 0)
        return "'c'";
    else if (originalChar.compare("'d'") == 0)
        return "'d'";
    else if (originalChar.compare("'e'") == 0)
        return "'e'";
    else if (originalChar.compare("'f'") == 0)
        return "'f'";
    else if (originalChar.compare("'g'") == 0)
        return "'g'";
    else if (originalChar.compare("'h'") == 0)
        return "'h'";
    else if (originalChar.compare("'i'") == 0)
        return "'i'";
    else if (originalChar.compare("'j'") == 0)
        return "'j'";
    else if (originalChar.compare("'k'") == 0)
        return "'k'";
    else if (originalChar.compare("'l'") == 0)
        return "'l'";
    else if (originalChar.compare("'m'") == 0)
        return "'m'";
    else if (originalChar.compare("'n'") == 0)
        return "'n'";
    else if (originalChar.compare("'o'") == 0)
        return "'o'";
    else if (originalChar.compare("'p'") == 0)
        return "'p'";
    else if (originalChar.compare("'q'") == 0)
        return "'q'";
    else if (originalChar.compare("'r'") == 0)
        return "'r'";
    else if (originalChar.compare("'s'") == 0)
        return "'s'";
    else if (originalChar.compare("'t'") == 0)
        return "'t'";
    else if (originalChar.compare("'u'") == 0)
        return "'u'";
    else if (originalChar.compare("'v'") == 0)
        return "'v'";
    else if (originalChar.compare("'w'") == 0)
        return "'w'";
    else if (originalChar.compare("'x'") == 0)
        return "'x'";
    else if (originalChar.compare("'y'") == 0)
        return "'y'";
    else if (originalChar.compare("'z'") == 0)
        return "'z'";
    else if (originalChar.compare("'{'") == 0)
        return "'{'";
    else if (originalChar.compare("'|'") == 0)
        return "'|'";
    else if (originalChar.compare("'}'") == 0)
        return "'}'";
    else if (originalChar.compare("'~'") == 0)
        return "'~'";
    else if (originalChar.compare("'\\n'") == 0)
        return "'\n'";
    else if (originalChar.compare("'\\a'") == 0)
        return "'\a'";
    else if (originalChar.compare("'\\b'") == 0)
        return "'\b'";
    else if (originalChar.compare("'\\f'") == 0)
        return "'\f'";
    else if (originalChar.compare("'\\r'") == 0)
        return "'\r'";
    else if (originalChar.compare("'\\t'") == 0)
        return "'\t'";
    else if (originalChar.compare("'\\v'") == 0)
        return "'\v'";

    stringstream ss;
    ss << "Unknown char literal " << originalChar;
    throw LexerException(ss.str());
}

void Lexer::ClearTokens()
{
    _tokens.resize(0);
}

}
