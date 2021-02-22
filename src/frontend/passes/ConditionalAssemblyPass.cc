#include "ConditionalAssemblyPass.h"

using namespace gbxasm::interfaces;
using namespace gbxasm::utilities;
using namespace std;

namespace gbxasm::frontend::passes
{

ConditionalAssemblyPass::ConditionalAssemblyPass(vector<string>& symbolTable)
    : _symbolTable(symbolTable)
{}

void ConditionalAssemblyPass::Process(string input)
{
    _workString = input;
    ProcessInput();
    ProcessBlocks();
}

string ConditionalAssemblyPass::Result()
{
    return _workString;
}

void ConditionalAssemblyPass::ProcessBlocks()
{
    for (auto block : _conditionalAssemblyBlocks)
    {
        auto blockToKeep = DetectBlockToKeep(block);
        RemoveBlock(blockToKeep, block);
    }
}

void ConditionalAssemblyPass::ProcessInput()
{
    auto currentLine = string("");
    auto lexeme = string("");    
    size_t globalCounter = 0llu;
    stringstream lineStream(_workString);
    stack<ConditionalAssemblyBlock> blocksStack;

    while(getline(lineStream, currentLine, '\n'))
    {
        currentLine += ' ';
        stringstream stream(currentLine);
    
        while (stream >> lexeme)
            ProcessDirective(lexeme, stream, blocksStack, globalCounter);

        globalCounter += currentLine.size();
    }

    if (blocksStack.size() != 0)
        throw PreProcessorException("Malformed conditional assembly block (expected '.END')");
}

inline BlockToKeepInCode ConditionalAssemblyPass::DetectBlockToKeep(ConditionalAssemblyBlock block)
{
    auto symbol = block.PreProcessorSymbol;
    auto position = find(begin(_symbolTable), end(_symbolTable), symbol);

    if (block.Type == BlockType::IfDef)
    {
        if (position == end(_symbolTable) && block.ElseBlock == nullopt)
            return BlockToKeepInCode::NoBlock;
        else if (position == end(_symbolTable) && block.ElseBlock != nullopt)
            return BlockToKeepInCode::ElseBlock;
        else if (position != end(_symbolTable))
            return BlockToKeepInCode::IfBlock;
    }
    else
    {
        if (position != end(_symbolTable) && block.ElseBlock == nullopt)
            return BlockToKeepInCode::NoBlock;
        else if (position != end(_symbolTable) && block.ElseBlock != nullopt)
            return BlockToKeepInCode::ElseBlock;
        else if (position == end(_symbolTable))
            return BlockToKeepInCode::IfBlock;
    }

    return BlockToKeepInCode::NoBlock;
}


inline void ConditionalAssemblyPass::RemoveBlock(BlockToKeepInCode targetBlockToKeep, ConditionalAssemblyBlock block)
{
    auto begin = 0llu;
    auto end = 0llu;
    auto endBegin = 0llu;
    auto endEnd = 0llu;
    auto ifBegin = block.IfBlock.BlockInitializerPosition;
    auto ifEnd = block.IfBlock.BlockPositionAfterSymbol;

    if (targetBlockToKeep == BlockToKeepInCode::IfBlock && block.ElseBlock == nullopt)
    {
        endBegin = block.IfBlock.BlockFinalizerPosition - Lexemes::PreProcessorEND.size();
        endEnd = block.IfBlock.BlockFinalizerPosition;
    }
    else if (targetBlockToKeep == BlockToKeepInCode::IfBlock  && block.ElseBlock != nullopt)
    {
        begin = block.ElseBlock.value().BlockInitializerPosition;
        end = block.ElseBlock.value().BlockFinalizerPosition;
    }
    else if (targetBlockToKeep == BlockToKeepInCode::ElseBlock)
    {
        begin = block.IfBlock.BlockInitializerPosition;
        end = block.IfBlock.BlockFinalizerPosition;

        endBegin = block.ElseBlock.value().BlockFinalizerPosition - Lexemes::PreProcessorEND.size();
        endEnd = block.ElseBlock.value().BlockFinalizerPosition;
    }
    else if (targetBlockToKeep == BlockToKeepInCode::NoBlock)
    {
        begin = block.IfBlock.BlockInitializerPosition;
        end = block.IfBlock.BlockFinalizerPosition;

        endBegin = block.IfBlock.BlockFinalizerPosition - Lexemes::PreProcessorEND.size();
        endEnd = block.IfBlock.BlockFinalizerPosition;
    }
    
    for (auto i = begin; i < end; ++i)
        if (_workString[i] != '\n')
            _workString[i] = ' ';
    
    for (auto i = ifBegin; i < ifEnd; ++i)
        if (_workString[i] != '\n')
            _workString[i] = ' ';
    
    for (auto i = endBegin; i < endEnd; ++i)
        if (_workString[i] != '\n')
            _workString[i] = ' ';
}

inline void ConditionalAssemblyPass::ProcessDirective(string_view lexeme, stringstream& stream, stack<ConditionalAssemblyBlock>& blocksStack, size_t globalCounter)
{
    auto upperCaseLexeme = string(lexeme);
    transform(begin(upperCaseLexeme), end(upperCaseLexeme), begin(upperCaseLexeme), [](unsigned char c) -> auto {return toupper(c);});

    if (upperCaseLexeme.compare(Lexemes::PreProcessorIFDEF.c_str()) == 0 ||
        upperCaseLexeme.compare(Lexemes::PreProcessorIFNDEF.c_str()) == 0)
        EvaluateIfDirectives(upperCaseLexeme, stream, blocksStack, globalCounter);
    else if (upperCaseLexeme.compare(Lexemes::PreProcessorEND.c_str()) == 0)
        EvaluateEnd(stream, blocksStack, globalCounter);
    else if (upperCaseLexeme.compare(Lexemes::PreProcessorELSE.c_str()) == 0)
        EvaluateElse(stream, blocksStack, globalCounter);
    else if (upperCaseLexeme.compare(Lexemes::PreProcessorDEF.c_str()) == 0)
        EvaluateDef(stream, globalCounter);
}

inline void ConditionalAssemblyPass::EvaluateIfDirectives(string lexeme, stringstream& stream, stack<ConditionalAssemblyBlock>& blocksStack, size_t globalCounter)
{
    auto targetBlock = lexeme.compare(Lexemes::PreProcessorIFDEF) == 0? BlockType::IfDef : BlockType::IfNDef;
    auto initialPosition = static_cast<size_t>(stream.tellg()) - lexeme.size();
    auto identifier = string("");
    stream >> identifier;                

    if (identifier.size() == 0)
    {
        stringstream ss;
        ss << "Malformed '" << lexeme << "' directive (identifier expected)";
        throw PreProcessorException(ss.str());
    }

    if (!IdentifierValidator::IsValid(identifier))
    {
        stringstream ss;
        ss << "Invalid Pre-Assembly symbol identifier '" << identifier << "'";
        throw PreProcessorException(ss.str());
    }

    ConditionalAssemblyBlock block = 
    {
        .Type = targetBlock,
        .PreProcessorSymbol = identifier,
        .IfBlock = 
        {
            .BlockInitializerPosition = globalCounter + static_cast<size_t>(initialPosition),
            .BlockPositionAfterSymbol = globalCounter + static_cast<size_t>(stream.tellg()),
            .BlockFinalizerPosition = 0llu,
        },
        .ElseBlock = nullopt
    };
    
    blocksStack.push(block);
}

inline void ConditionalAssemblyPass::EvaluateEnd(stringstream& stream, stack<ConditionalAssemblyBlock>& blocksStack, size_t globalCounter)
{
    if (blocksStack.size() == 0)
        throw PreProcessorException("Unexpected '.END' directive found");

    auto block = blocksStack.top();
    blocksStack.pop();

    if (block.ElseBlock == nullopt)
        block.IfBlock.BlockFinalizerPosition = globalCounter + static_cast<size_t>(stream.tellg());
    else
        block.ElseBlock.value().BlockFinalizerPosition = globalCounter + static_cast<size_t>(stream.tellg());

    _conditionalAssemblyBlocks.push_back(block);
}

inline void ConditionalAssemblyPass::EvaluateElse(stringstream& stream, stack<ConditionalAssemblyBlock>& blocksStack, size_t globalCounter)
{
    if (blocksStack.size() == 0)
        throw PreProcessorException("Unexpected '.ELSE' directive found");

    auto topBlock = blocksStack.top();
    blocksStack.pop();
    topBlock.IfBlock.BlockFinalizerPosition = globalCounter + static_cast<size_t>(stream.tellg());

    BlockLimits block = 
    {
        .BlockInitializerPosition = globalCounter + static_cast<size_t>(stream.tellg()) - Lexemes::PreProcessorELSE.size(),
        .BlockPositionAfterSymbol = 0llu,
        .BlockFinalizerPosition = 0llu,
    };

    topBlock.ElseBlock = make_optional(block);
    blocksStack.push(topBlock);
}

inline void ConditionalAssemblyPass::EvaluateDef(stringstream& stream, size_t globalCounter)
{
    auto initialPosition = static_cast<size_t>(stream.tellg()) - Lexemes::PreProcessorDEF.size();
    auto identifier = string("");
    stream >> identifier;                

    if (identifier.size() == 0)
        throw PreProcessorException("Malformed '.DEF' directive (identifier expected)");

    if (!IdentifierValidator::IsValid(identifier))
    {
        stringstream ss;
        ss << "Invalid Pre-Assembly symbol identifier '" << identifier << "'";
        throw PreProcessorException(ss.str());
    }

    // Remove DEF Directive from the code
    auto begin = globalCounter + static_cast<size_t>(initialPosition);
    auto end = globalCounter + static_cast<size_t>(stream.tellg());
    
    for (auto i = begin; i < end; ++i)
        if (_workString[i] != '\n')
            _workString[i] = ' ';

    _symbolTable.push_back(identifier);
}

}