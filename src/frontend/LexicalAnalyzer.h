#pragma once

#include <algorithm>
#include <limits>
#include <vector>
#include <sstream>
#include <string_view>
#include <utility>

#include <iostream>

#include "../utilities/IdentifierValidator.h"
#include "../GBXAsmExceptions.h"
#include "Lexemes.h"
#include "Token.h"

namespace gbxasm
{

class LexicalAnalyzer
{
public:
    LexicalAnalyzer();
    ~LexicalAnalyzer() = default;

    void Tokenize(std::string_view);
    std::vector<Token>& Tokens();

private:
    std::vector<Token> EvaluateLexeme(std::string, size_t, size_t);
    std::vector<std::pair<std::string, size_t> > FindSubLexemes(std::string, size_t);
    inline std::string ExtractOperatorSeparatorOrMarker(std::string, size_t);
    inline std::string ExtractOperator(std::string, size_t);
    inline std::string ExtractSeparator(std::string, size_t);
    inline std::string ExtractStringLiteralMarker(std::string, size_t);
    inline std::string ExtractCharLiteralFromCandidate(std::string, size_t);
    
    void ClearTokens();
    void ExtractTokens(std::string_view);

    inline bool IsPossibleOperator(std::string_view, size_t);
    inline bool IsPossibleSeparator(std::string_view, size_t);
    inline bool IsPossibleStringLiteralMarker(std::string_view, size_t);
    inline bool IsPossibleCharLiteralMarker(std::string_view, size_t);
    inline bool IsSeparatorOrOperator(std::string_view, size_t);
    inline bool IsNumericLiteral(std::string_view);
    inline bool IsStringLiteral(std::string_view);
    inline bool IsCharLiteral(std::string_view);
    inline bool IsDigit(std::string_view, size_t);
    inline bool HasUnmergedStrings();

    inline TokenType IdentifyNumericLiteral(std::string_view);
    inline TokenType ParseStringLiteral(std::string_view);
    
    inline Token GenerateStringToken(size_t, size_t, std::string_view);
    inline void ValidateDecimalLiteral(std::string_view);
    inline void ValidateOctalLiteral(std::string_view);
    inline void ValidateBinaryLiteral(std::string_view);
    inline void EvaluateStringLimits(std::string, size_t);
    inline void ExtractAllStringTokenIfNeeded(std::string_view);
    inline void ClearStringLimitFlags();
    
    inline void ConvertCharLiteral(std::vector<std::pair<std::string, size_t> >&);
    
    inline size_t FindTokenByType(TokenType, size_t);
    
    inline std::string AccumulateFirstSeparatorOfCharLiteral(std::string, size_t&);
    inline std::string AccumulateSecondSeparatorOfCharLiteral(std::string, size_t&);
    
    inline static void SaveSubLexeme(std::string, size_t, std::vector<std::pair<std::string, size_t> >&, size_t&);
    inline static void CorrectLoopIndex(std::vector<std::pair<std::string, size_t> >&, size_t&); 
    inline static void EvaluateCharLiteralSize(std::string);

    inline static std::string AccumulateContentOfCharLiteral(std::string, size_t&);
    inline static std::string ExtractPossibleSubCharLiteral(std::string, size_t);
    inline static std::string EvaluateAndConvertChar(std::string_view);

    inline void EvaluateIdentifier(std::string);

    std::vector<Token> _tokens;
    bool _stringLiteralAccumulationStarted{};
    bool _stringLiteralAccumulationEnded{};
};

}