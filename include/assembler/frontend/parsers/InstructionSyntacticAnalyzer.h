#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include "ConstructionSyntacticAnalyzer.h"
#include "GBXAsmExceptions.h"
#include "InstructionIntermediateRepresentation.h"
#include "Opcodes.h"

namespace gbxasm::frontend::parsers
{

enum class InstructionParserTreeSymbol
{
    // Operands
    TerminalIdentifier,
    TerminalComma,
    TerminalNumericLiteral,
    
    // Instruction Mnemonics
    TerminalInstructionLD,

    // Non Terminal
    NonTerminaOpcode,
};

enum class InstructionClass
{
    OneOperand,
    TwoOperands,
};

typedef struct InstructionCompoundSymbol_t
{
    std::string Lexeme;
    size_t Line;
    size_t Column;
    InstructionParserTreeSymbol Symbol;
}
InstructionCompoundSymbol;

class InstructionSyntacticAnalyzer : public interfaces::ConstructionSyntacticAnalyzer
{
public:
    InstructionSyntacticAnalyzer() = default;
    virtual ~InstructionSyntacticAnalyzer() = default;

    InstructionSyntacticAnalyzer(const InstructionSyntacticAnalyzer&) = default;
    InstructionSyntacticAnalyzer(InstructionSyntacticAnalyzer&&) = default;
    InstructionSyntacticAnalyzer& operator=(const InstructionSyntacticAnalyzer&) = default;
    InstructionSyntacticAnalyzer& operator=(InstructionSyntacticAnalyzer&&) = default;

    std::shared_ptr<gbxasm::intermediate_representation::IntermediateRepresentation> TryToAccept(std::vector<Token>::iterator&, std::vector<Token>::iterator&) override;

private:
    void ExtractSymbols(std::vector<Token>::iterator&, std::vector<Token>::iterator&);
    void ReduceOpcode(int);

    gbxcore::instructions::OpcodeType TerminalToOpcode(InstructionCompoundSymbol);
 
    inline void Shift(int&);

    std::vector<InstructionCompoundSymbol> _symbols;
    size_t _line;
    size_t _column;
};

}