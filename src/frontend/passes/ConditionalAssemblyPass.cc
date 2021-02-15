#include "ConditionalAssemblyPass.h"

using namespace gbxasm::interfaces;
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
}

string ConditionalAssemblyPass::Result()
{
    return string("");
}

void ConditionalAssemblyPass::LocateBlocks()
{
    stringstream lineStream(_workString);
    auto currentLine = string("");
    auto lexeme = string("");
    auto globalCounter = 0llu;

    stack<ConditionalAssemblyBlock> blocksStack;

    while(getline(lineStream, currentLine, '\n'))
    {
        // Add an extra space to allow the calculation of stream position by the end of e line.
        currentLine += ' ';
        stringstream stream(currentLine);
        
        while (stream >> lexeme)
        {
            auto upperCaseLexeme = lexeme;
            transform(begin(upperCaseLexeme), end(upperCaseLexeme), begin(upperCaseLexeme), [](unsigned char c) -> auto {return toupper(c);});

            if (upperCaseLexeme.compare(Lexemes::PreProcessorIFDEF.c_str()) == 0)
            {
                auto identifier = string("");
                stream >> identifier;                

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
            else if (upperCaseLexeme.compare(Lexemes::PreProcessorEND.c_str()) == 0)
            {
                auto block = blocksStack.top();
                blocksStack.pop();

                if (block.ElseBlock == nullopt)
                    block.IfBlock.BlockFinalizerPosition = globalCounter + static_cast<size_t>(stream.tellg()) - lexeme.size();
                else
                    block.ElseBlock.value().BlockFinalizerPosition = globalCounter + static_cast<size_t>(stream.tellg()) - lexeme.size();

                _conditionalAssemblyBlocks.push_back(block);
            }
            else if (upperCaseLexeme.compare(Lexemes::PreProcessorELSE.c_str()) == 0)
            {
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
        
        globalCounter += currentLine.size();
    }

}

}