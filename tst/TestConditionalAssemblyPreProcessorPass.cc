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


TEST(TestConditionalAssemblyPreProcessorPass, NesteIfDefBlock)
{
    string program = ".IFDEF THE_SYMBOL\n"
                     "\tLD A, 0xFF\n"
                     "\tINC A\n"
                     "\tSUB A, C"
                     "\n"
                     ".IFDEF THE_SECOND_SYMBOL\n"
                     "\tLD HL, 0xFF45\n"
                     "\tLD A, [HL]\n"
                     ".END\n"
                     ".END\n";

    string innerBlock = "\n\tLD HL, 0xFF45\n"
                        "\tLD A, [HL]\n";
    
    string OuterBlock = "\n\tLD A, 0xFF\n"
                        "\tINC A\n"
                        "\tSUB A, C"
                        "\n"
                        ".IFDEF THE_SECOND_SYMBOL\n"
                        "\tLD HL, 0xFF45\n"
                        "\tLD A, [HL]\n"
                        ".END\n";
    
    
    vector<string> symbolTable;
    symbolTable.push_back("THE_SYMBOL");
    symbolTable.push_back("THE_SECOND_SYMBOL");

    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    pass->Process(program);

    EXPECT_EQ(2llu, pass->Blocks().size());
    EXPECT_STREQ("THE_SECOND_SYMBOL", pass->Blocks()[0].PreProcessorSymbol.c_str());
    EXPECT_EQ(nullopt, pass->Blocks()[0].ElseBlock);
    EXPECT_STREQ("THE_SYMBOL", pass->Blocks()[1].PreProcessorSymbol.c_str());
    EXPECT_EQ(nullopt, pass->Blocks()[1].ElseBlock);

    auto processedInnerBlock = program.substr(pass->Blocks()[0].IfBlock.BlockInitializerPosition, pass->Blocks()[0].IfBlock.BlockFinalizerPosition - pass->Blocks()[0].IfBlock.BlockInitializerPosition);
    auto processedOuterBlock = program.substr(pass->Blocks()[1].IfBlock.BlockInitializerPosition, pass->Blocks()[1].IfBlock.BlockFinalizerPosition - pass->Blocks()[1].IfBlock.BlockInitializerPosition);
    
    EXPECT_STREQ(innerBlock.c_str(), processedInnerBlock.c_str());
    EXPECT_STREQ(OuterBlock.c_str(), processedOuterBlock.c_str());
}

TEST(TestConditionalAssemblyPreProcessorPass, MalformedConditionalAssemblyBlock)
{
    string program = ".IFDEF MY_SYMBOL\n"
                     "\tLD SP, 0xFFFE\n"
                     "\tEI\n";

    vector<string> symbolTable;
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Malformed '.IFDEF' conditional assembly block (expected '.END')");
}

TEST(TestConditionalAssemblyPreProcessorPass, UnexpectedEndKeyword)
{
    string program = ".IFDEF MY_SYMBOL\n"
                     "\tLD SP, 0xFFFE\n"
                     "\tEI\n"
                     ".END\n"
                     ".END\n";

    vector<string> symbolTable; 
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Unexpected '.END' directive found");
}

TEST(TestConditionalAssemblyPreProcessorPass, UnexpectedElseKeyword)
{
    string program = ".ELSE\n"
                     "\tLD SP, 0xFFFE\n"
                     "\tEI\n";

    vector<string> symbolTable; 
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Unexpected '.ELSE' directive found");
}

TEST(TestConditionalAssemblyPreProcessorPass, MalformedIfDefDirective)
{
    string program = ".IFDEF\n"
                     "\tLD SP, 0xFFFE\n"
                     "\tEI\n";

    vector<string> symbolTable; 
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Malformed '.IFDEF' directive (identifier expected)");
}

TEST(TestConditionalAssemblyPreProcessorPass, InvalidIdentifier)
{
    string program = ".IFDEF ^&&~AAAA\n"
                     "\tLD SP, 0xFFFE\n"
                     "\tEI\n";

    vector<string> symbolTable; 
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Invalid PreAssembly symbol identifier '^&&~AAAA'");
}
