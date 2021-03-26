#pragma once

#include <iostream>
#include <limits>
#include <memory>
#include <vector>

#include "ConstructionSyntacticAnalyzer.h"
#include "DeclaredMember.h"
#include "FUNCIntermediateRepresentation.h"
#include "Lexemes.h"
#include "LexemeToDeclaredMemberType.h"
#include "Token.h"

namespace gbxasm::frontend::parsers
{

enum class FUNCParseTreeSymbols
{
    TerminalFunc,
    TerminalIdentifier,
    TerminalBgn,
    TerminalIn,
    TerminalSemicolon,
    TerminalOut,
    TerminalAs,
    TerminalType,
    TerminalOpenBracket,
    TerminalNumericLiteral,
    TerminalCloseBracket,
    TerminalEnd,
    TerminalIgnore,
    NonTerminalHeader,
    NonTerminalArgument,
    NonTerminalArgumentList,
    NonTerminalBody,
    NonTerminalFunc,
};

typedef struct FUNCCompoundSymbol_t
{
    std::string Lexeme;
    FUNCParseTreeSymbols Symbol;
}
FUNCCompoundSymbol;

class FUNCSyntacticAnalyzer : public interfaces::ConstructionSyntacticAnalyzer
{
public:
    FUNCSyntacticAnalyzer() = default;
    virtual ~FUNCSyntacticAnalyzer() = default;

    FUNCSyntacticAnalyzer(const FUNCSyntacticAnalyzer&) = default;
    FUNCSyntacticAnalyzer(FUNCSyntacticAnalyzer&&) = default;
    FUNCSyntacticAnalyzer& operator=(const FUNCSyntacticAnalyzer&) = default;
    FUNCSyntacticAnalyzer& operator=(FUNCSyntacticAnalyzer&&) = default;

    std::shared_ptr<gbxasm::intermediate_representation::IntermediateRepresentation> TryToAccept(std::vector<Token>::iterator&, std::vector<Token>::iterator&) override;

private:
    enum class FSMStates
    {
        InitialState,
        DetectFUNC,
        DetectFUNCIdentifier,
        ReduceFUNCHeader,
        ArgumentsInitialDetection,
        IgnoreBody,
        ReduceFUNCBody,
        DetectColon,
        DetectArgumentIdentifier,
        ReduceArgumentOrDetectType,
        DetectArgumentType,
        ReduceTypeOrDetectArray,
        DetectArrayDimensions,
        DetectClosedBrackets,
        ReduceArgumentOrMergeArgumentList,
        MergeArgumentList,
        ReduceArgumentList,
    };

    inline void Shift(int&);
    void ExtractSymbols(std::vector<Token>::iterator&, std::vector<Token>::iterator&);
    size_t CountEndWithinFunctionBody(std::vector<Token>::iterator&, std::vector<Token>::iterator&);

    void MergeArgumentsList(int&);
    void ReduceHeader(int&, std::string&);
    void ReduceBody(int&);
    void ReduceBody(std::vector<FUNCCompoundSymbol>::iterator, int&);
    void ReduceTypelessArgument(int&);
    void ReduceNonArrayArgument(int&);
    void ReduceArrayArgument(int&);
    void ReduceArgumentList(int&);
    void ReduceFUNC(int&);

    void ExtractTypelessArgumentInfo(int&, std::vector<intermediate_representation::DeclaredArgument>&, std::vector<intermediate_representation::DeclaredArgument>&);
    void ExtractNonArrayArgumentInfo(int&, std::vector<intermediate_representation::DeclaredArgument>&, std::vector<intermediate_representation::DeclaredArgument>&);
    void ExtractArrayArgumentInfo(int&, std::vector<intermediate_representation::DeclaredArgument>&, std::vector<intermediate_representation::DeclaredArgument>&);

    std::vector<FUNCCompoundSymbol>::iterator FindFunctionBodyBegin();

    std::vector<FUNCCompoundSymbol> _symbols;
    size_t _line;
    size_t _column;
    std::vector<Token> _bodyTokens;
};

}