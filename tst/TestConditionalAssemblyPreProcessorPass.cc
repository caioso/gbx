#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestUtils.h"

#include <memory>
#include <string>
#include <vector>

#include "../src/frontend/passes/ConditionalAssemblyPass.h"
#include "../src/interfaces/Pass.h"
#include "../src/GBXAsmExceptions.h"

using namespace gbxasm;
using namespace gbxasm::interfaces;
using namespace gbxasm::frontend;
using namespace gbxasm::frontend::passes;
using namespace gbxasm::utilities;
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

    string ifBlock = ".IFDEF MY_SYMBOL\n"
                     "\tLD SP, 0xFFFE\n"
                     "\tEI\n"
                     ".END";

    vector<string> symbolTable;
    symbolTable.push_back("MY_SYMBOL");

    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    pass->Process(program);

    EXPECT_EQ(1llu, pass->Blocks().size());
    EXPECT_STREQ("MY_SYMBOL", pass->Blocks()[0].PreProcessorSymbol.c_str());
    EXPECT_EQ(nullopt, pass->Blocks()[0].ElseBlock);
    EXPECT_EQ(BlockType::IfDef, pass->Blocks()[0].Type);

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

    string ifBlock = ".IFDEF THE_SYMBOL\n"
                     "\tLD A, 0xFF\n"
                     "\tINC A\n"
                     ".ELSE";
    
    string elseBlock = ".ELSE\n"
                     "\tCALL MY_FUNCTION\n"
                     "\tRET\n"
                     ".END";

    vector<string> symbolTable;
    symbolTable.push_back("THE_SYMBOL");

    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    pass->Process(program);

    EXPECT_EQ(1llu, pass->Blocks().size());
    EXPECT_STREQ("THE_SYMBOL", pass->Blocks()[0].PreProcessorSymbol.c_str());
    EXPECT_NE(nullopt, pass->Blocks()[0].ElseBlock);
    EXPECT_EQ(BlockType::IfDef, pass->Blocks()[0].Type);

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

    string innerBlock = ".IFDEF THE_SECOND_SYMBOL\n"
                     "\tLD HL, 0xFF45\n"
                     "\tLD A, [HL]\n"
                     ".END";
    
    string OuterBlock = ".IFDEF THE_SYMBOL\n"
                     "\tLD A, 0xFF\n"
                     "\tINC A\n"
                     "\tSUB A, C"
                     "\n"
                     ".IFDEF THE_SECOND_SYMBOL\n"
                     "\tLD HL, 0xFF45\n"
                     "\tLD A, [HL]\n"
                     ".END\n"
                     ".END";
    
    
    vector<string> symbolTable;
    symbolTable.push_back("THE_SYMBOL");
    symbolTable.push_back("THE_SECOND_SYMBOL");

    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    pass->Process(program);

    EXPECT_EQ(2llu, pass->Blocks().size());
    EXPECT_STREQ("THE_SECOND_SYMBOL", pass->Blocks()[0].PreProcessorSymbol.c_str());
    EXPECT_EQ(BlockType::IfDef, pass->Blocks()[0].Type);
    EXPECT_EQ(nullopt, pass->Blocks()[0].ElseBlock);
    EXPECT_STREQ("THE_SYMBOL", pass->Blocks()[1].PreProcessorSymbol.c_str());
    EXPECT_EQ(nullopt, pass->Blocks()[1].ElseBlock);
    EXPECT_EQ(BlockType::IfDef, pass->Blocks()[1].Type);

    auto processedInnerBlock = program.substr(pass->Blocks()[0].IfBlock.BlockInitializerPosition, pass->Blocks()[0].IfBlock.BlockFinalizerPosition - pass->Blocks()[0].IfBlock.BlockInitializerPosition);
    auto processedOuterBlock = program.substr(pass->Blocks()[1].IfBlock.BlockInitializerPosition, pass->Blocks()[1].IfBlock.BlockFinalizerPosition - pass->Blocks()[1].IfBlock.BlockInitializerPosition);
    
    EXPECT_STREQ(innerBlock.c_str(), processedInnerBlock.c_str());
    EXPECT_STREQ(OuterBlock.c_str(), processedOuterBlock.c_str());
}

TEST(TestConditionalAssemblyPreProcessorPass, MalformedIfDefConditionalAssemblyBlock)
{
    string program = ".IFDEF MY_SYMBOL\n"
                     "\tLD SP, 0xFFFE\n"
                     "\tEI\n";

    vector<string> symbolTable;
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Malformed conditional assembly block (expected '.END')");
}

TEST(TestConditionalAssemblyPreProcessorPass, UnexpectedIfDefAndTwoEndKeyword)
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

TEST(TestConditionalAssemblyPreProcessorPass, MalformedIfNDefConditionalAssemblyBlock)
{
    string program = ".IFNDEF MY_SYMBOL\n"
                     "\tLD SP, 0xFFFE\n"
                     "\tEI\n";

    vector<string> symbolTable;
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Malformed conditional assembly block (expected '.END')");
}

TEST(TestConditionalAssemblyPreProcessorPass, UnexpectedIfNDefAndTwoEndKeyword)
{
    string program = ".IFNDEF MY_SYMBOL\n"
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

TEST(TestConditionalAssemblyPreProcessorPass, MalformedIfNDefDirective)
{
    string program = ".IFNDEF\n"
                     "\tLD SP, 0xFFFE\n"
                     "\tEI\n";

    vector<string> symbolTable; 
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Malformed '.IFNDEF' directive (identifier expected)");
}

TEST(TestConditionalAssemblyPreProcessorPass, MalformedIfDefDirective2)
{
    string program = ".IFDEF DEFINED\n"
                     "\tSLA A, 1\n"
                     ".IFDEF NO_DEFINED\n"
                     "\tLD C, A\n"
                     "\tLD C, A\n"
                     "\tLD C, A\n"
                     ".ELSE\n"
                     "\tLD C, A\n"
                     ".END\n"
                     "\tSUB A, B\n"
                     ".END\n"
                     "\tSLA A, 1\n"
                     "\tLD C, A\n"
                     ".END\n";

    vector<string> symbolTable; 
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                       "Unexpected '.END' directive found");
}

TEST(TestConditionalAssemblyPreProcessorPass, InvalidIfDefIdentifier)
{
    string program = ".IFDEF ^&&~AAAA\n"
                     "\tLD SP, 0xFFFE\n"
                     "\tEI\n";

    vector<string> symbolTable; 
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Invalid Pre-Assembly symbol identifier '^&&~AAAA'");
}

TEST(TestConditionalAssemblyPreProcessorPass, InvalidIfNDefIdentifier)
{
    string program = ".IFNDEF %#@!!@AAAAA\n"
                     "\tLD DE, %#@!!@AAAAA\n"
                     "\tLD A, 0x00\n";

    vector<string> symbolTable; 
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Invalid Pre-Assembly symbol identifier '%#@!!@AAAAA'");
}

TEST(TestConditionalAssemblyPreProcessorPass, TestCodeRemovalIfDef)
{
    string program = ".IFDEF THE_SYMBOL\n"
                     "\tLD A, 0xFF\n"
                     "\tINC A\n"
                     ".ELSE\n"
                     "\tCALL MY_FUNCTION\n"
                     "\tRET\n"
                     ".END\n";
    
    string result =  "                 \n"
                     "\tLD A, 0xFF\n"
                     "\tINC A\n"
                     "     \n"
                     "                 \n"
                     "    \n"
                     "    \n";
    
    string SecondPassResult =  "                 \n"
                     "           \n"
                     "      \n"
                     "     \n"
                     "\tCALL MY_FUNCTION\n"
                     "\tRET\n"
                     "    \n";

    vector<string> symbolTable;
    symbolTable.push_back("THE_SYMBOL");

    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    pass->Process(program);
    auto processedCode = pass->Result();

    EXPECT_STREQ(result.c_str(), processedCode.c_str());
    
    symbolTable.pop_back();

    auto secondPass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    secondPass->Process(program);
    auto secondPassProcessedCode = secondPass->Result();

    EXPECT_STREQ(SecondPassResult.c_str(), secondPassProcessedCode.c_str());
}

TEST(TestConditionalAssemblyPreProcessorPass, TestRemovalIfDef2)
{
    string program = ".IFDEF NO_DEFINED\n"
                     "\tSLA A, 1\n"
                     "\tLD C, A\n"
                     ".END\n";
    
    string result = "                 \n"
                     "         \n"
                     "        \n"
                     "    \n";

    vector<string> symbolTable; 
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    pass->Process(program);
    auto processedCode = pass->Result();

    EXPECT_STREQ(result.c_str(), processedCode.c_str());
}

TEST(TestConditionalAssemblyPreProcessorPass, TestRemovalIfDef3)
{
    string program = ".IFDEF DEFINED\n"
                     "\tSLA A, 1\n"
                     ".IFDEF NO_DEFINED\n"
                     "\tLD C, A\n"
                     "\tLD C, A\n"
                     "\tLD C, A\n"
                     ".ELSE\n"
                     "\tLD C, A\n"
                     ".END\n"
                     "\tSUB A, B\n"
                     ".END\n";
    
    string result = "              \n"
                     "\tSLA A, 1\n"
                     "                 \n"
                     "        \n"
                     "        \n"
                     "        \n"
                     "     \n"
                     "\tLD C, A\n"
                     "    \n"
                     "\tSUB A, B\n"
                     "    \n";

    vector<string> symbolTable; 
    symbolTable.push_back("DEFINED");
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    pass->Process(program);
    auto processedCode = pass->Result();

    EXPECT_STREQ(result.c_str(), processedCode.c_str());
}

TEST(TestConditionalAssemblyPreProcessorPass, TestRemovalIfDef4)
{
    string program = ".IFDEF DEFINED\n"
                     "\tSLA A, 1\n"
                     ".IFDEF NO_DEFINED\n"
                     "\tLD C, A\n"
                     "\tLD C, A\n"
                     "\tLD C, A\n"
                     ".ELSE\n"
                     "\tLD C, A\n"
                     ".END\n"
                     "\tSUB A, B\n"
                     ".ELSE\n"
                     "\tSLA A, 1\n"
                     "\tLD C, A\n"
                     ".END\n";
    
    string result = "              \n"
                     "\tSLA A, 1\n"
                     "                 \n"
                     "        \n"
                     "        \n"
                     "        \n"
                     "     \n"
                     "\tLD C, A\n"
                     "    \n"
                     "\tSUB A, B\n"
                     "     \n"
                     "         \n"
                     "        \n"
                     "    \n";

    vector<string> symbolTable; 
    symbolTable.push_back("DEFINED");
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    pass->Process(program);
    auto processedCode = pass->Result();

    EXPECT_STREQ(result.c_str(), processedCode.c_str());
}

TEST(TestConditionalAssemblyPreProcessorPass, IfNDefBlockDetectionWithoutElse)
{
    string program = ".IFNDEF MY_UNDEFINED_SYMBOL\n"
                     "LD A, 0xEF\n"
                     "SLA\n"
                     ".END";

    string ifBlock = ".IFNDEF MY_UNDEFINED_SYMBOL\n"
                     "LD A, 0xEF\n"
                     "SLA\n"
                     ".END";

    vector<string> symbolTable;
    symbolTable.push_back("MY_SYMBOL");

    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    pass->Process(program);

    EXPECT_EQ(1llu, pass->Blocks().size());
    EXPECT_EQ(BlockType::IfNDef, pass->Blocks()[0].Type);
    EXPECT_STREQ("MY_UNDEFINED_SYMBOL", pass->Blocks()[0].PreProcessorSymbol.c_str());
    EXPECT_EQ(nullopt, pass->Blocks()[0].ElseBlock);

    auto processedIfBlock = program.substr(pass->Blocks()[0].IfBlock.BlockInitializerPosition, pass->Blocks()[0].IfBlock.BlockFinalizerPosition - pass->Blocks()[0].IfBlock.BlockInitializerPosition);
    EXPECT_STREQ(ifBlock.c_str(), processedIfBlock.c_str());
}

TEST(TestConditionalAssemblyPreProcessorPass, IfNDefBlockDetectionWithElse)
{
    string program = ".IFNDEF THE_UNDEFINED_SYMBOL\n"
                     "\tLD C, 0xFF\n"
                     "\tLD A, 0xFF\n"
                     "\tADD A, C\n"
                     ".ELSE\n"
                     "\tLD C, 0x00\n"
                     "\tLD A, 0x01\n"
                     "\tADD A, C\n"
                     ".END\n";

    string ifBlock = ".IFNDEF THE_UNDEFINED_SYMBOL\n"
                     "\tLD C, 0xFF\n"
                     "\tLD A, 0xFF\n"
                     "\tADD A, C\n"
                     ".ELSE";
    
    string elseBlock = ".ELSE\n"
                     "\tLD C, 0x00\n"
                     "\tLD A, 0x01\n"
                     "\tADD A, C\n"
                     ".END";

    vector<string> symbolTable;
    symbolTable.push_back("THE_SYMBOL");

    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    pass->Process(program);

    EXPECT_EQ(1llu, pass->Blocks().size());
    EXPECT_STREQ("THE_UNDEFINED_SYMBOL", pass->Blocks()[0].PreProcessorSymbol.c_str());
    EXPECT_NE(nullopt, pass->Blocks()[0].ElseBlock);
    EXPECT_EQ(BlockType::IfNDef, pass->Blocks()[0].Type);

    auto processedIfBlock = program.substr(pass->Blocks()[0].IfBlock.BlockInitializerPosition, pass->Blocks()[0].IfBlock.BlockFinalizerPosition - pass->Blocks()[0].IfBlock.BlockInitializerPosition);
    auto processedElseBlock = program.substr(pass->Blocks()[0].ElseBlock.value().BlockInitializerPosition, pass->Blocks()[0].ElseBlock.value().BlockFinalizerPosition - pass->Blocks()[0].ElseBlock.value().BlockInitializerPosition);
    
    EXPECT_STREQ(ifBlock.c_str(), processedIfBlock.c_str());
    EXPECT_STREQ(elseBlock.c_str(), processedElseBlock.c_str());
}

TEST(TestConditionalAssemblyPreProcessorPass, TestCodeRemovalIfNDef)
{
    string program = ".IFNDEF THE_SYMBOL\n"
                     "\tLD HL, 0x77FA\n"
                     "\tLD A, [HL]\n"
                     ".ELSE\n"
                     "\tINC A\n"
                     "\tJP MY_LABEL\n"
                     ".END\n";
    
    string result =  "                  \n"
                     "\tLD HL, 0x77FA\n"
                     "\tLD A, [HL]\n"
                     "     \n"
                     "      \n"
                     "            \n"
                     "    \n";
    
    string SecondPassResult =  "                  \n"
                     "              \n"
                     "           \n"
                     "     \n"
                     "\tINC A\n"
                     "\tJP MY_LABEL\n"
                     "    \n";

    vector<string> symbolTable;
    symbolTable.push_back("THE_OTHER_SYMBOL");

    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    pass->Process(program);
    auto processedCode = pass->Result();

    EXPECT_STREQ(result.c_str(), processedCode.c_str());
    
    symbolTable.push_back("THE_SYMBOL");

    auto secondPass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    secondPass->Process(program);
    auto secondPassProcessedCode = secondPass->Result();

    EXPECT_STREQ(SecondPassResult.c_str(), secondPassProcessedCode.c_str());
}

TEST(TestConditionalAssemblyPreProcessorPass, TestRemovalIfNDef2)
{
    string program = ".IFNDEF NO_DEFINED\n"
                     "\tSLA A, 1\n"
                     "\tLD C, A\n"
                     ".END\n";
    
    string result = "                  \n"
                     "\tSLA A, 1\n"
                     "\tLD C, A\n"
                     "    \n";

    vector<string> symbolTable; 
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    pass->Process(program);
    auto processedCode = pass->Result();

    EXPECT_STREQ(result.c_str(), processedCode.c_str());
}

TEST(TestConditionalAssemblyPreProcessorPass, TestRemovalIfNDef3)
{
    string program = ".IFNDEF DEFINED\n"
                     "\tSLA A, 1\n"
                     ".IFNDEF NO_DEFINED\n"
                     "\tLD C, A\n"
                     "\tLD C, A\n"
                     "\tLD C, A\n"
                     ".ELSE\n"
                     "\tLD C, A\n"
                     ".END\n"
                     "\tSUB A, B\n"
                     ".END\n";
    
    string result = "               \n"
                     "\tSLA A, 1\n"
                     "                  \n"
                     "        \n"
                     "        \n"
                     "        \n"
                     "     \n"
                     "\tLD C, A\n"
                     "    \n"
                     "\tSUB A, B\n"
                     "    \n";

    vector<string> symbolTable; 
    symbolTable.push_back("NO_DEFINED");
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    pass->Process(program);
    auto processedCode = pass->Result();

    EXPECT_STREQ(result.c_str(), processedCode.c_str());
}

TEST(TestConditionalAssemblyPreProcessorPass, TestRemovalIfNDef4)
{
    string program = ".IFNDEF DEFINED\n"
                     "\tSLA A, 1\n"
                     ".IFNDEF NO_DEFINED\n"
                     "\tLD C, A\n"
                     "\tLD C, A\n"
                     "\tLD C, A\n"
                     ".ELSE\n"
                     "\tLD C, A\n"
                     ".END\n"
                     "\tSUB A, B\n"
                     ".ELSE\n"
                     "\tSLA A, 1\n"
                     "\tLD C, A\n"
                     ".END\n";
    
    string result = "               \n"
                     "\tSLA A, 1\n"
                     "                  \n"
                     "        \n"
                     "        \n"
                     "        \n"
                     "     \n"
                     "\tLD C, A\n"
                     "    \n"
                     "\tSUB A, B\n"
                     "     \n"
                     "         \n"
                     "        \n"
                     "    \n";

    vector<string> symbolTable; 
    symbolTable.push_back("NO_DEFINED");
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    pass->Process(program);
    auto processedCode = pass->Result();

    EXPECT_STREQ(result.c_str(), processedCode.c_str());
}

TEST(TestConditionalAssemblyPreProcessorPass, SymbolDefintion)
{
    string program = ".DEF MY_SYMBOL\n";
    string output = "              \n";

    vector<string> symbolTable; 
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable);
    pass->Process(program);
    auto processedCode = pass->Result();

    EXPECT_EQ(1llu, symbolTable.size());
    EXPECT_STREQ("MY_SYMBOL", symbolTable[0].c_str());
    EXPECT_STREQ(output.c_str(), processedCode.c_str());
}