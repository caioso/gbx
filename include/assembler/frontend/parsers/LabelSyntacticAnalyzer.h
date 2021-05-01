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
    void ExtractSymbols(std::vector<Token>::iterator&, std::vector<Token>::iterator&);

    std::vector<LabelCompoundSymbol> _symbols;
    size_t _line;
    size_t _column;
};

}