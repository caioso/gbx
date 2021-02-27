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

namespace gbxasm::frontend::passes
{

enum class BlockType
{
    IfDef,
    IfNDef
};

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
    BlockType Type;
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
    void ProcessInput();
    void ProcessBlocks();

    inline void ProcessDirective(std::string_view, std::stringstream&, std::stack<ConditionalAssemblyBlock>&, size_t);
    inline void EvaluateIfDirectives(std::string, std::stringstream&, std::stack<ConditionalAssemblyBlock>&, size_t);
    inline void EvaluateEnd(std::stringstream&, std::stack<ConditionalAssemblyBlock>&, size_t);
    inline void EvaluateElse(std::stringstream&, std::stack<ConditionalAssemblyBlock>&, size_t);
    inline void EvaluateDef(std::stringstream&, size_t);
    inline void EvaluateUndef(std::stringstream&, size_t);
    inline void RemoveBlock(BlockToKeepInCode, ConditionalAssemblyBlock);

    inline BlockToKeepInCode DetectBlockToKeep(ConditionalAssemblyBlock);
    inline void EvaluateIdentifier(std::string, std::string);
    inline void RemoveSingleLineDirective(size_t, size_t, size_t);
    inline void RemoveIdentifierFromSymbolTable(std::string);
    inline void AddIdentifierToSymbolTable(std::string);

    std::vector<ConditionalAssemblyBlock> _conditionalAssemblyBlocks;
    std::vector<std::string>& _symbolTable;
    std::string _workString;
};

}