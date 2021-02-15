#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestUtils.h"

#include <memory>
#include <string>
#include <vector>

#include "../src/frontend/passes/ConditionalAssemblyPass.h"
#include "../src/interfaces/Pass.h"
#include "../src/GBXAsmExceptions.h"

using namespace gbxasm::interfaces;
using namespace gbxasm;
using namespace std;

class ConditionalAssemblyPassWrapper : public ConditionalAssemblyPass
{
public:
    ConditionalAssemblyPassWrapper(vector<string>& symbolTable)
        : ConditionalAssemblyPass(symbolTable)
    {}

    std::vector<ConditionalAssemblyBlock> Blocks()
    {
        return _conditionalAssemblyBlocks;
    }
};

TEST(TestConditionalAssemblyPreProcessorPass, Construction)
{
    vector<string> symbolTable;
    auto pass = make_shared<ConditionalAssemblyPass>(symbolTable);
}

TEST(TestConditionalAssemblyPreProcessorPass, IfDefBlockDetectionWithoutElse)
{
    string program = ".IFDEF MY_SYMBOL\n"
                     "\tLD SP, 0xFFFE\n"
                     "\tEI\n"
                     ".END";

    string ifBlock = "\n\tLD SP, 0xFFFE\n"
                     "\tEI\n";

    vector<string> symbolTable;
    symbolTable.push_back("MY_SYMBOL");

    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    pass->Process(program);

    EXPECT_EQ(1llu, pass->Blocks().size());
    EXPECT_STREQ("MY_SYMBOL", pass->Blocks()[0].PreProcessorSymbol.c_str());
    EXPECT_EQ(nullopt, pass->Blocks()[0].ElseBlock);

    auto processedIfBlock = program.substr(pass->Blocks()[0].IfBlock.BlockInitializerPosition, pass->Blocks()[0].IfBlock.BlockFinalizerPosition - pass->Blocks()[0].IfBlock.BlockInitializerPosition);
    EXPECT_STREQ(ifBlock.c_str(), processedIfBlock.c_str());
}

TEST(TestConditionalAssemblyPreProcessorPass, IfDefBlockDetectionWithElse)
{
    string program = ".IFDEF THE_SYMBOL\n"
                     "\tLD A, 0xFF\n"
                     "\tINC A\n"
                     ".ELSE\n"
                     "\tCALL MY_FUNCTION\n"
                     "\tRET\n"
                     ".END\n";

    string ifBlock = "\n\tLD A, 0xFF\n"
                     "\tINC A\n";
    
    string elseBlock = "\n\tCALL MY_FUNCTION\n"
                       "\tRET\n";

    vector<string> symbolTable;
    symbolTable.push_back("THE_SYMBOL");

    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    pass->Process(program);

    EXPECT_EQ(1llu, pass->Blocks().size());
    EXPECT_STREQ("THE_SYMBOL", pass->Blocks()[0].PreProcessorSymbol.c_str());
    EXPECT_NE(nullopt, pass->Blocks()[0].ElseBlock);

    auto processedIfBlock = program.substr(pass->Blocks()[0].IfBlock.BlockInitializerPosition, pass->Blocks()[0].IfBlock.BlockFinalizerPosition - pass->Blocks()[0].IfBlock.BlockInitializerPosition);
    auto processedElseBlock = program.substr(pass->Blocks()[0].ElseBlock.value().BlockInitializerPosition, pass->Blocks()[0].ElseBlock.value().BlockFinalizerPosition - pass->Blocks()[0].ElseBlock.value().BlockInitializerPosition);
    
    EXPECT_STREQ(ifBlock.c_str(), processedIfBlock.c_str());
    EXPECT_STREQ(elseBlock.c_str(), processedElseBlock.c_str());
}