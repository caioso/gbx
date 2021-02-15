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

    std::vector<std::string>& _symbolTable;
    std::vector<ConditionalAssemblyBlock> _conditionalAssemblyBlocks;
    std::string _workString;
};

}