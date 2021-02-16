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
    return string("");
}

void ConditionalAssemblyPass::ProcessBlocks()
{
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
        EvaluateEnd(stream, blocksStack, globalCounter, lexeme);
    else if (upperCaseLexeme.compare(Lexemes::PreProcessorELSE.c_str()) == 0)
        EvaluateElse(stream, blocksStack, globalCounter, lexeme);
}

inline void ConditionalAssemblyPass::EvaluateIfDef(stringstream& stream, stack<ConditionalAssemblyBlock>& blocksStack, size_t globalCounter)
{
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
            .BlockInitializerPosition = globalCounter + static_cast<size_t>(stream.tellg()),
            .BlockFinalizerPosition = 0llu
        },
        .ElseBlock = nullopt
    };
    
    blocksStack.push(block);
}

inline void ConditionalAssemblyPass::EvaluateEnd(stringstream& stream, stack<ConditionalAssemblyBlock>& blocksStack, size_t globalCounter,  string_view lexeme)
{
    if (blocksStack.size() == 0)
        throw PreProcessorException("Unexpected '.END' directive found");

    auto block = blocksStack.top();
    blocksStack.pop();

    if (block.ElseBlock == nullopt)
        block.IfBlock.BlockFinalizerPosition = globalCounter + static_cast<size_t>(stream.tellg()) - lexeme.size();
    else
        block.ElseBlock.value().BlockFinalizerPosition = globalCounter + static_cast<size_t>(stream.tellg()) - lexeme.size();

    _conditionalAssemblyBlocks.push_back(block);
}

inline void ConditionalAssemblyPass::EvaluateElse(stringstream& stream, stack<ConditionalAssemblyBlock>& blocksStack, size_t globalCounter, string_view lexeme)
{
    if (blocksStack.size() == 0)
        throw PreProcessorException("Unexpected '.ELSE' directive found");

    auto topBlock = blocksStack.top();
    blocksStack.pop();
    topBlock.IfBlock.BlockFinalizerPosition = globalCounter + static_cast<size_t>(stream.tellg()) - lexeme.size();

    BlockLimits block = 
    {
        .BlockInitializerPosition = globalCounter + static_cast<size_t>(stream.tellg()),
        .BlockFinalizerPosition = 0llu
    };

    topBlock.ElseBlock = make_optional(block);
    blocksStack.push(topBlock);
}

}