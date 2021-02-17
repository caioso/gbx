#include "ConditionalAssemblyPass.h"

using namespace gbxasm::interfaces;
using namespace gbxasm::utilities;
using namespace std;

namespace gbxasm
{

ConditionalAssemblyPass::ConditionalAssemblyPass(vector<string>& symbolTable)
    : _symbolTable(symbolTable)
{}

void ConditionalAssemblyPass::Process(string input)
{
    _workString = input;
    LocateBlocks();
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

inline BlockToKeepInCode ConditionalAssemblyPass::DetectBlockToKeep(ConditionalAssemblyBlock block)
{
    auto symbol = block.PreProcessorSymbol;
    auto position = find(begin(_symbolTable), end(_symbolTable), symbol);

    if (position == end(_symbolTable) && block.ElseBlock == nullopt)
        return BlockToKeepInCode::NoBlock;
    else if (position == end(_symbolTable) && block.ElseBlock != nullopt)
        return BlockToKeepInCode::ElseBlock;
    else if (position != end(_symbolTable))
        return BlockToKeepInCode::IfBlock;

    return BlockToKeepInCode::NoBlock;
}

void ConditionalAssemblyPass::LocateBlocks()
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
        throw PreProcessorException("Malformed '.IFDEF' conditional assembly block (expected '.END')");
}

inline void ConditionalAssemblyPass::ProcessDirective(string_view lexeme, stringstream& stream, stack<ConditionalAssemblyBlock>& blocksStack, size_t globalCounter)
{
    auto upperCaseLexeme = string(lexeme);
    transform(begin(upperCaseLexeme), end(upperCaseLexeme), begin(upperCaseLexeme), [](unsigned char c) -> auto {return toupper(c);});

    if (upperCaseLexeme.compare(Lexemes::PreProcessorIFDEF.c_str()) == 0)
        EvaluateIfDef(stream, blocksStack, globalCounter);
    else if (upperCaseLexeme.compare(Lexemes::PreProcessorEND.c_str()) == 0)
        EvaluateEnd(stream, blocksStack, globalCounter);
    else if (upperCaseLexeme.compare(Lexemes::PreProcessorELSE.c_str()) == 0)
        EvaluateElse(stream, blocksStack, globalCounter);
}

inline void ConditionalAssemblyPass::EvaluateIfDef(stringstream& stream, stack<ConditionalAssemblyBlock>& blocksStack, size_t globalCounter)
{
    auto initialPosition = static_cast<size_t>(stream.tellg()) - Lexemes::PreProcessorIFDEF.size();
    auto identifier = string("");
    stream >> identifier;                

    if (identifier.size() == 0)
        throw PreProcessorException("Malformed '.IFDEF' directive (identifier expected)");

    if (!IdentifierValidator::IsValid(identifier))
    {
        stringstream ss;
        ss << "Invalid PreAssembly symbol identifier '" << identifier << "'";
        throw PreProcessorException(ss.str());
    }

    ConditionalAssemblyBlock block = 
    {
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

}