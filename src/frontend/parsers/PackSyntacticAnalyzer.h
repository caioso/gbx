#pragma once

#include <algorithm>
#include <vector>
#include <sstream>
#include <stack>

#include "../../GBXAsmExceptions.h"
#include "../../interfaces/ConstructionSyntacticAnalyzer.h"
#include "../../intermediate-representation/IntermediateRepresentation.h"
#include "../../intermediate-representation/PackIntermediateRepresentation.h"
#include "../../intermediate-representation/PackIntermediateRepresentation.h"
#include "../../intermediate-representation/DeclaredMember.h"
#include "../../utilities/LexemeToDeclaredMemberType.h"

namespace gbxasm::frontend::parsers
{

enum class PackParseTreeSymbols
{
    TerminalPack,
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
        PackHeaderDetection = 3,
        InitialMemberDetectionOrFooterOrNTPackDetection = 4,
        MemberIdentifierDetection = 5,
        MemberArrayOpenBracketDetection = 6,
        ArrayDimensionDetection = 7,
        MemberArrayCloseBracketDetection = 8,
        MemberArrayDetection = 9,
        MemberListOrFooterOrNTPackDetection = 10,
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