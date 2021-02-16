#pragma once

#include <algorithm>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <stack>
#include <vector>

#include <iostream>

#include "../../interfaces/Pass.h"
#include "../../utilities/IdentifierValidator.h"
#include "../../GBXAsmExceptions.h"
#include "../Lexemes.h"

namespace gbxasm
{

struct BlockLimits
{
    size_t BlockInitializerPosition;
    size_t BlockFinalizerPosition;
};

struct ConditionalAssemblyBlock
{
    std::string PreProcessorSymbol;
    BlockLimits IfBlock;
    
    std::optional<BlockLimits> ElseBlock;
};

class ConditionalAssemblyPass : public interfaces::Pass
{ 
public:
    ConditionalAssemblyPass(std::vector<std::string>&);
    virtual ~ConditionalAssemblyPass() = default;

    virtual void Process(std::string) override;
    [[nodiscard]] virtual std::string Result() override;

protected:
    void LocateBlocks();
    void ProcessBlocks();

    inline void ProcessDirective(std::string_view, std::stringstream&, std::stack<ConditionalAssemblyBlock>&, size_t);
    inline void EvaluateIfDef(std::stringstream&, std::stack<ConditionalAssemblyBlock>&, size_t);
    inline void EvaluateEnd(std::stringstream&, std::stack<ConditionalAssemblyBlock>&, size_t, std::string_view);
    inline void EvaluateElse(std::stringstream&, std::stack<ConditionalAssemblyBlock>&, size_t, std::string_view);

    std::vector<std::string>& _symbolTable;
    std::vector<ConditionalAssemblyBlock> _conditionalAssemblyBlocks;
    std::string _workString;
};

}