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
#include "PackIntermediateRepresentation.h"
#include "DeclaredMember.h"
#include "LexemeToDeclaredMemberType.h"

namespace gbxasm::frontend::parsers
{

enum class PackParseTreeSymbols
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

typedef struct PackCompoundSymbol_t
{
    std::string Lexeme;
    PackParseTreeSymbols Symbol;
}
PackCompoundSymbol;

class PackSyntacticAnalyzer : public interfaces::ConstructionSyntacticAnalyzer
{
public:
    PackSyntacticAnalyzer() = default;
    virtual ~PackSyntacticAnalyzer() = default;

    PackSyntacticAnalyzer(const PackSyntacticAnalyzer&) = default;
    PackSyntacticAnalyzer(PackSyntacticAnalyzer&&) = default;
    PackSyntacticAnalyzer& operator=(const PackSyntacticAnalyzer&) = default;
    PackSyntacticAnalyzer& operator=(PackSyntacticAnalyzer&&) = default;

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

    std::vector<PackCompoundSymbol> _symbols;
};

}