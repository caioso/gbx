#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include "LabelIntermediateRepresentation.h"
#include "ConstructionSyntacticAnalyzer.h"
#include "GBXAsmExceptions.h"

namespace gbxasm::frontend::parsers
{

enum class LabelParserTreeSymbol
{
    TerminalIdentifier,
    TerminalIgnore,
    TerminalSemiColon,
    TerminalLessThan,
    TerminalGBL,
    TerminalLOC,
    TerminalGreaterThan,

    NonTerminalLabelName,
    NonTerminalSimpleLabel,
    NonTerminalGloballyDefinedLabel,
    NonTerminalLocallyDefinedLabel,
    NonTerminalGlobalScopeMarker,
    NonTerminalLocalScopeMarker,

    NonTerminalLabel
};

typedef struct LabelCompoundSymbol_t
{
    std::string Lexeme;
    size_t Line;
    size_t Column;
    LabelParserTreeSymbol Symbol;
}
LabelCompoundSymbol;

class LabelSyntacticAnalyzer : public interfaces::ConstructionSyntacticAnalyzer
{
public:
    LabelSyntacticAnalyzer() = default;
    virtual ~LabelSyntacticAnalyzer() = default;

    LabelSyntacticAnalyzer(const LabelSyntacticAnalyzer&) = default;
    LabelSyntacticAnalyzer(LabelSyntacticAnalyzer&&) = default;
    LabelSyntacticAnalyzer& operator=(const LabelSyntacticAnalyzer&) = default;
    LabelSyntacticAnalyzer& operator=(LabelSyntacticAnalyzer&&) = default;

    std::shared_ptr<gbxasm::intermediate_representation::IntermediateRepresentation> TryToAccept(std::vector<Token>::iterator&, std::vector<Token>::iterator&) override;

private:
    enum class FSMState
    {
        InitialState,
        IdentifyLabelOrAcceptLabel,
        DetectSimpleLabelOrMarkedLabel,
        ReduceSimpleLabel,
        ReduceLabel,
        DetectMakedLabelType,
        DetectMakedEnd,
        ReduceGlobalMarker,
        ReduceLocalMarker,
        ReduceFullGlobalLabel,
        ReduceFullLocalLabel,
    };

    void ExtractSymbols(std::vector<Token>::iterator&, std::vector<Token>::iterator&);
    void ReduceSimpleLabel(int);
    void ReduceLabelName(int, std::string&);
    void ReduceLabel(int);
    void ReduceGlobalMarker(int);
    void ReduceLocalMarker(int);
    void ReduceFullGlobalLabel(int);
    void ReduceFullLocalLabel(int);
    inline void Shift(int&);

    bool IsPartiallyReducedLabel(int);
    bool IsSemiColonOrMarkerBegin(int);
    
    std::vector<LabelCompoundSymbol> _symbols;
    size_t _line;
    size_t _column;
};

}