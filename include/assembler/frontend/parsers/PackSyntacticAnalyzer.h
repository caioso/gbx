#pragma once

#include <algorithm>
#include <iostream>
#include <vector>
#include <sstream>
#include <stack>

#include "GBXAsmExceptions.h"
#include "ConstructionSyntacticAnalyzer.h"
#include "IntermediateRepresentation.h"
#include "PackIntermediateRepresentation.h"
#include "DeclaredMember.h"
#include "LexemeToDeclaredMemberType.h"

namespace gbxasm::frontend::parsers
{

enum class PACKParseTreeSymbols
{
    TerminalPack,
    TerminalBgn,
    TerminalType,
    TerminalIdentifier,
    TerminalOpenBracket,
    TerminalNumericLiteral,
    TerminalCloseBracket,
    TerminalEnd,
    NonTerminalHeader,
    NonTerminalMember,
    NonTerminalMemberList,
    NonTerminalFooter,
    NonTerminalPack,
};

typedef struct PACKCompoundSymbol_t
{
    std::string Lexeme;
    PACKParseTreeSymbols Symbol;
}
PACKCompoundSymbol;

class PACKSyntacticAnalyzer : public interfaces::ConstructionSyntacticAnalyzer
{
public:
    PACKSyntacticAnalyzer() = default;
    virtual ~PACKSyntacticAnalyzer() = default;

    PACKSyntacticAnalyzer(const PACKSyntacticAnalyzer&) = default;
    PACKSyntacticAnalyzer(PACKSyntacticAnalyzer&&) = default;
    PACKSyntacticAnalyzer& operator=(const PACKSyntacticAnalyzer&) = default;
    PACKSyntacticAnalyzer& operator=(PACKSyntacticAnalyzer&&) = default;

    std::shared_ptr<gbxasm::intermediate_representation::IntermediateRepresentation> TryToAccept(std::vector<Token>::iterator&, std::vector<Token>::iterator&) override;

private:
    enum FSMStates : int
    {
        InitialState = 0,
        InitialPackHeaderOrNTPackDetection = 1,
        PackIdentifierDetection = 2,
        PackBgnDetection = 3,
        PackHeaderDetection = 4,
        InitialMemberDetectionOrFooterOrNTPackDetection = 5,
        MemberIdentifierDetection = 6,
        NonArrayMemberReduction = 7,
        ArrayDimensionDetection = 8,
        MemberArrayCloseBracketDetection = 9,
        MemberTypeAndDimensionDetection = 10,
        MemberListOrFooterOrNTPackDetection = 11,
        MemberArrayIdentifierDetected = 12,
    };

    void ExtractSymbols(std::vector<Token>::iterator&, std::vector<Token>::iterator&);
    std::shared_ptr<gbxasm::intermediate_representation::IntermediateRepresentation> ExtractConstructions();

    inline void Shift(int&);
    inline void ReduceHeader(int, std::string&);
    inline void ReduceEmptyPackFooter(int);
    inline void ReduceEmptyPack(int);
    inline void ReduceMember(int, intermediate_representation::DeclaredMember&, std::vector<intermediate_representation::DeclaredMember>&);
    inline void ReduceArrayMember(int, intermediate_representation::DeclaredMember&, std::vector<intermediate_representation::DeclaredMember>&);
    inline void ReduceMemberList(int);
    inline void ReduceFooter(int);
    inline void ReducePack(int);

    std::vector<PACKCompoundSymbol> _symbols;

    size_t _endLine{};
    size_t _endColumn{};
};

}