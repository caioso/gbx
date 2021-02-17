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

enum class BlockToKeepInCode
{
    IfBlock,
    ElseBlock,
    NoBlock
};

struct BlockLimits
{
    size_t BlockInitializerPosition;
    size_t BlockPositionAfterSymbol;
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
    inline void EvaluateEnd(std::stringstream&, std::stack<ConditionalAssemblyBlock>&, size_t);
    inline void EvaluateElse(std::stringstream&, std::stack<ConditionalAssemblyBlock>&, size_t);
    inline void RemoveBlock(BlockToKeepInCode, ConditionalAssemblyBlock);

    inline BlockToKeepInCode DetectBlockToKeep(ConditionalAssemblyBlock);

    std::vector<ConditionalAssemblyBlock> _conditionalAssemblyBlocks;
    std::vector<std::string>& _symbolTable;
    std::string _workString;
};

}