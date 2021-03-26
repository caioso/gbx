#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "AssemblerTestMocks.h"
#include "TestUtils.h"

#include <memory>
#include <string>
#include <vector>

#include "ConditionalAssemblyPass.h"
#include "Pass.h"
#include "GBXAsmExceptions.h"

using namespace gbxasm;
using namespace gbxasm::interfaces;
using namespace gbxasm::frontend;
using namespace gbxasm::frontend::passes;
using namespace gbxasm::utilities;
using namespace std;

using ::testing::Return;
using ::testing::_;

class ConditionalAssemblyPassWrapper : public ConditionalAssemblyPass
{
public:
    ConditionalAssemblyPassWrapper(vector<string>& symbolTable, std::shared_ptr<interfaces::MessageStream> stream)
        : ConditionalAssemblyPass(symbolTable, stream)
    {}

    std::vector<ConditionalAssemblyBlock> Blocks()
    {
        return _conditionalAssemblyBlocks;
    }
};

TEST(AssemblerTests_ConditionalAssemblyPass, Construction)
{
    vector<string> symbolTable;
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
}

TEST(AssemblerTests_ConditionalAssemblyPass, IfDefBlockDetectionWithoutElse)
{
    string program = ".IFDEF MY_SYMBOL\n"
                     ".BGN\n"
                     "\tLD SP, 0xFFFE\n"
                     "\tEI\n"
                     ".END";

    string ifBlock = ".IFDEF MY_SYMBOL\n"
                     ".BGN\n"
                     "\tLD SP, 0xFFFE\n"
                     "\tEI\n"
                     ".END";

    vector<string> symbolTable;
    symbolTable.push_back("MY_SYMBOL");

    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    pass->Process(program);

    EXPECT_EQ(1llu, pass->Blocks().size());
    EXPECT_STREQ("MY_SYMBOL", pass->Blocks()[0].PreProcessorSymbol.c_str());
    EXPECT_EQ(nullopt, pass->Blocks()[0].ElseBlock);
    EXPECT_EQ(BlockType::IfDef, pass->Blocks()[0].Type);

    auto processedIfBlock = program.substr(pass->Blocks()[0].IfBlock.BlockInitializerPosition, pass->Blocks()[0].IfBlock.BlockFinalizerPosition - pass->Blocks()[0].IfBlock.BlockInitializerPosition);
    EXPECT_STREQ(ifBlock.c_str(), processedIfBlock.c_str());
}

TEST(AssemblerTests_ConditionalAssemblyPass, IfDefBlockDetectionWithoutElse2)
{
    string program = ".IFDEF MY_SYMBOL .BGN \tLD SP, 0xFFFE \tEI .END";

    string result =  "                      \tLD SP, 0xFFFE \tEI     ";

    vector<string> symbolTable;
    symbolTable.push_back("MY_SYMBOL");

    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    pass->Process(program);
    auto processedCode = pass->Result();

    EXPECT_STREQ(result.c_str(), processedCode.c_str());
}

TEST(AssemblerTests_ConditionalAssemblyPass, IfDefBlockDetectionWithElse)
{
    string program = ".IFDEF THE_SYMBOL\n"
                     ".BGN\n"    
                     "\tLD A, 0xFF\n"
                     "\tINC A\n"
                     ".ELSE\n"
                     ".BGN\n"
                     "\tCALL MY_FUNCTION\n"
                     "\tRET\n"
                     ".END\n";

    string ifBlock = ".IFDEF THE_SYMBOL\n"
                     ".BGN\n"    
                     "\tLD A, 0xFF\n"
                     "\tINC A\n"
                     ".ELSE\n"
                     ".BGN"; 
    
    string elseBlock = ".ELSE\n"
                     ".BGN\n"    
                     "\tCALL MY_FUNCTION\n"
                     "\tRET\n"
                     ".END";

    vector<string> symbolTable;
    symbolTable.push_back("THE_SYMBOL");

    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
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

TEST(AssemblerTests_ConditionalAssemblyPass, IfDefBlockDetectionWithElse2)
{
    string program = ".IFDEF MY_SYMBOL .BGN LD SP, 0xFFFE EI .ELSE .BGN PUSH A LD A, 0xEE .END";

    string result =  "                                                  PUSH A LD A, 0xEE     ";

    vector<string> symbolTable;
    symbolTable.push_back("MY_OTHER_SYMBOL");

    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    pass->Process(program);
    auto processedCode = pass->Result();

    EXPECT_STREQ(result.c_str(), processedCode.c_str());
}

TEST(AssemblerTests_ConditionalAssemblyPass, NesteIfDefBlock)
{
    string program = ".IFDEF THE_SYMBOL\n"
                     ".BGN\n"
                     "\tLD A, 0xFF\n"
                     "\tINC A\n"
                     "\tSUB A, C"
                     "\n"
                     ".IFDEF THE_SECOND_SYMBOL\n"
                     ".BGN\n"                     
                     "\tLD HL, 0xFF45\n"
                     "\tLD A, [HL]\n"
                     ".END\n"
                     ".END\n";

    string innerBlock = ".IFDEF THE_SECOND_SYMBOL\n"
                     ".BGN\n"    
                     "\tLD HL, 0xFF45\n"
                     "\tLD A, [HL]\n"
                     ".END";
    
    string OuterBlock = ".IFDEF THE_SYMBOL\n"
                     ".BGN\n"    
                     "\tLD A, 0xFF\n"
                     "\tINC A\n"
                     "\tSUB A, C"
                     "\n"
                     ".IFDEF THE_SECOND_SYMBOL\n"
                     ".BGN\n" 
                     "\tLD HL, 0xFF45\n"
                     "\tLD A, [HL]\n"
                     ".END\n"
                     ".END";
    
    
    vector<string> symbolTable;
    symbolTable.push_back("THE_SYMBOL");
    symbolTable.push_back("THE_SECOND_SYMBOL");

    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
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

TEST(AssemblerTests_ConditionalAssemblyPass, MalformedIfDefConditionalAssemblyBlock)
{
    string program = ".IFDEF MY_SYMBOL\n"
                     ".BGN\n"    
                     "\tLD SP, 0xFFFE\n"
                     "\tEI\n";

    vector<string> symbolTable;
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Malformed conditional assembly block (expected '.END')");
}

TEST(AssemblerTests_ConditionalAssemblyPass, UnexpectedIfDefAndTwoEndKeyword)
{
    string program = ".IFDEF MY_SYMBOL\n"
                     ".BGN\n"    
                     "\tLD SP, 0xFFFE\n"
                     "\tEI\n"
                     ".END\n"
                     ".END\n";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Unexpected '.END' directive found");
}

TEST(AssemblerTests_ConditionalAssemblyPass, MalformedIfDefWithoutBgn)
{
    string program = ".IFDEF MY_SYMBOL\n"
                     "\tLD SP, 0xFFFE\n"
                     "\tEI\n"
                     ".END\n";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Malformed '.IFDEF' directive (expected '.BGN')");
}

TEST(AssemblerTests_ConditionalAssemblyPass, MalformedIfNDefWithoutBgn)
{
    string program = ".IFNDEF MY_SYMBOL\n"
                     "\tLD SP, 0xFFFE\n"
                     "\tEI\n"
                     ".END\n";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Malformed '.IFNDEF' directive (expected '.BGN')");
}

TEST(AssemblerTests_ConditionalAssemblyPass, MalformedElseInIfDefWithoutBgn)
{
    string program = ".IFDEF MY_SYMBOL\n"
                     ".BGN\n"      
                     "\tLD A, 0xFF\n"
                     ".ELSE"
                     "\tLD B, 0x00\n"
                     ".END\n";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Malformed '.ELSE' directive (expected '.BGN')");
}

TEST(AssemblerTests_ConditionalAssemblyPass, MalformedElseInIfNDefWithoutBgn)
{
    string program = ".IFNDEF MY_SYMBOL\n"
                     ".BGN\n"      
                     "\tLD A, 0xFF\n"
                     ".ELSE"
                     "\tLD B, 0x00\n"
                     ".END\n";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Malformed '.ELSE' directive (expected '.BGN')");
}

TEST(AssemblerTests_ConditionalAssemblyPass, MalformedIfNDefConditionalAssemblyBlock)
{
    string program = ".IFNDEF MY_SYMBOL\n"
                     ".BGN\n"        
                     "\tLD SP, 0xFFFE\n"
                     "\tEI\n";

    vector<string> symbolTable;
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Malformed conditional assembly block (expected '.END')");
}

TEST(AssemblerTests_ConditionalAssemblyPass, UnexpectedIfNDefAndTwoEndKeyword)
{
    string program = ".IFNDEF MY_SYMBOL\n"
                     ".BGN\n"        
                     "\tLD SP, 0xFFFE\n"
                     "\tEI\n"
                     ".END\n"
                     ".END\n";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Unexpected '.END' directive found");
}

TEST(AssemblerTests_ConditionalAssemblyPass, UnexpectedElseKeyword)
{
    string program = ".ELSE\n"
                     ".BGN\n"                
                     "\tLD SP, 0xFFFE\n"
                     "\tEI\n";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Unexpected '.ELSE' directive found");
}

TEST(AssemblerTests_ConditionalAssemblyPass, MalformedIfDefDirective)
{
    string program = ".IFDEF\n"
                     ".BGN\n"        
                     "\tLD SP, 0xFFFE\n"
                     "\tEI\n";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Malformed '.IFDEF' directive (identifier expected)");
}

TEST(AssemblerTests_ConditionalAssemblyPass, MalformedIfNDefDirective)
{
    string program = ".IFNDEF\n"
                     ".BGN\n"        
                     "\tLD SP, 0xFFFE\n"
                     "\tEI\n";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Malformed '.IFNDEF' directive (identifier expected)");
}

TEST(AssemblerTests_ConditionalAssemblyPass, MalformedIfDefDirective2)
{
    string program = ".IFDEF DEFINED\n"
                     ".BGN\n"        
                     "\tSLA A, 1\n"
                     ".IFDEF NO_DEFINED\n"
                     ".BGN\n"                         
                     "\tLD C, A\n"
                     "\tLD C, A\n"
                     "\tLD C, A\n"
                     ".ELSE\n"
                     ".BGN\n"                     
                     "\tLD C, A\n"
                     ".END\n"
                     "\tSUB A, B\n"
                     ".END\n"
                     "\tSLA A, 1\n"
                     "\tLD C, A\n"
                     ".END\n";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                       "Unexpected '.END' directive found");
}

TEST(AssemblerTests_ConditionalAssemblyPass, InvalidIfDefIdentifier)
{
    string program = ".IFDEF ^&&~AAAA\n"
                     ".BGN\n"        
                     "\tLD SP, 0xFFFE\n"
                     "\tEI\n";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Invalid Pre-Assembly symbol identifier '^&&~AAAA'");
}

TEST(AssemblerTests_ConditionalAssemblyPass, InvalidIfNDefIdentifier)
{
    string program = ".IFNDEF %#@!!@AAAAA\n"
                     ".BGN\n"        
                     "\tLD DE, %#@!!@AAAAA\n"
                     "\tLD A, 0x00\n";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    
    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Invalid Pre-Assembly symbol identifier '%#@!!@AAAAA'");
}

TEST(AssemblerTests_ConditionalAssemblyPass, TestCodeRemovalIfDef)
{
    string program = ".IFDEF THE_SYMBOL\n"
                     ".BGN\n"        
                     "\tLD A, 0xFF\n"
                     "\tINC A\n"
                     ".ELSE\n"
                     ".BGN\n"                     
                     "\tCALL MY_FUNCTION\n"
                     "\tRET\n"
                     ".END\n";
    
    string result =  "                 \n"
                     "    \n"
                     "\tLD A, 0xFF\n"
                     "\tINC A\n"
                     "     \n"
                     "    \n"                     
                     "                 \n"
                     "    \n"
                     "    \n";
    
    string SecondPassResult = "                 \n"
                              "    \n"        
                              "           \n"
                              "      \n"
                              "     \n"
                              "    \n"                     
                              "\tCALL MY_FUNCTION\n"
                              "\tRET\n"
                              "    \n"; 


    vector<string> symbolTable;
    symbolTable.push_back("THE_SYMBOL");

    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    pass->Process(program);
    auto processedCode = pass->Result();

    EXPECT_STREQ(result.c_str(), processedCode.c_str());
    
    symbolTable.pop_back();

    auto secondPass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, stream);
    secondPass->Process(program);
    auto secondPassProcessedCode = secondPass->Result();

    EXPECT_STREQ(SecondPassResult.c_str(), secondPassProcessedCode.c_str());
}

TEST(AssemblerTests_ConditionalAssemblyPass, TestRemovalIfDef2)
{
    string program = ".IFDEF NO_DEFINED\n"
                     ".BGN\n" 
                     "\tSLA A, 1\n"
                     "\tLD C, A\n"
                     ".END\n";
    
    string result = "                 \n"
                     "    \n" 
                     "         \n"
                     "        \n"
                     "    \n";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    pass->Process(program);
    auto processedCode = pass->Result();

    EXPECT_STREQ(result.c_str(), processedCode.c_str());
}

TEST(AssemblerTests_ConditionalAssemblyPass, TestRemovalIfDef3)
{
    string program = ".IFDEF DEFINED\n"
                     ".BGN\n" 
                     "\tSLA A, 1\n"
                     ".IFDEF NO_DEFINED\n"
                     ".BGN\n"                      
                     "\tLD C, A\n"
                     "\tLD C, A\n"
                     "\tLD C, A\n"
                     ".ELSE\n"
                     ".BGN\n"                     
                     "\tLD C, A\n"
                     ".END\n"
                     "\tSUB A, B\n"
                     ".END\n";
    
    string result = "              \n"
                     "    \n"     
                     "\tSLA A, 1\n"
                     "                 \n"
                     "    \n"                      
                     "        \n"
                     "        \n"
                     "        \n"
                     "     \n"
                     "    \n"                     
                     "\tLD C, A\n"
                     "    \n"
                     "\tSUB A, B\n"
                     "    \n";

    vector<string> symbolTable; 
    symbolTable.push_back("DEFINED");
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    pass->Process(program);
    auto processedCode = pass->Result();

    EXPECT_STREQ(result.c_str(), processedCode.c_str());
}

TEST(AssemblerTests_ConditionalAssemblyPass, TestRemovalIfDef4)
{
    string program = ".IFDEF DEFINED\n"
                     ".BGN\n"     
                     "\tSLA A, 1\n"
                     ".IFDEF NO_DEFINED\n"
                     ".BGN\n"                      
                     "\tLD C, A\n"
                     "\tLD C, A\n"
                     "\tLD C, A\n"
                     ".ELSE\n"
                     ".BGN\n"                     
                     "\tLD C, A\n"
                     ".END\n"
                     "\tSUB A, B\n"
                     ".ELSE\n"
                     ".BGN\n"                     
                     "\tSLA A, 1\n"
                     "\tLD C, A\n"
                     ".END\n";
    
    string result = "              \n"
                     "    \n"     
                     "\tSLA A, 1\n"
                     "                 \n"
                     "    \n"                      
                     "        \n"
                     "        \n"
                     "        \n"
                     "     \n"
                     "    \n"                     
                     "\tLD C, A\n"
                     "    \n"
                     "\tSUB A, B\n"
                     "     \n"
                     "    \n"                     
                     "         \n"
                     "        \n"
                     "    \n";

    vector<string> symbolTable; 
    symbolTable.push_back("DEFINED");
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    pass->Process(program);
    auto processedCode = pass->Result();

    EXPECT_STREQ(result.c_str(), processedCode.c_str());
}

TEST(AssemblerTests_ConditionalAssemblyPass, IfNDefBlockDetectionWithoutElse)
{
    string program = ".IFNDEF MY_UNDEFINED_SYMBOL\n"
                     ".BGN\n"     
                     "LD A, 0xEF\n"
                     "SLA\n"
                     ".END";

    string ifBlock = ".IFNDEF MY_UNDEFINED_SYMBOL\n"
                     ".BGN\n"     
                     "LD A, 0xEF\n"
                     "SLA\n"
                     ".END";

    vector<string> symbolTable;
    symbolTable.push_back("MY_SYMBOL");

    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    pass->Process(program);

    EXPECT_EQ(1llu, pass->Blocks().size());
    EXPECT_EQ(BlockType::IfNDef, pass->Blocks()[0].Type);
    EXPECT_STREQ("MY_UNDEFINED_SYMBOL", pass->Blocks()[0].PreProcessorSymbol.c_str());
    EXPECT_EQ(nullopt, pass->Blocks()[0].ElseBlock);

    auto processedIfBlock = program.substr(pass->Blocks()[0].IfBlock.BlockInitializerPosition, pass->Blocks()[0].IfBlock.BlockFinalizerPosition - pass->Blocks()[0].IfBlock.BlockInitializerPosition);
    EXPECT_STREQ(ifBlock.c_str(), processedIfBlock.c_str());
}

TEST(AssemblerTests_ConditionalAssemblyPass, IfNDefBlockDetectionWithElse)
{
    string program = ".IFNDEF THE_UNDEFINED_SYMBOL\n"
                     ".BGN\n"     
                     "\tLD C, 0xFF\n"
                     "\tLD A, 0xFF\n"
                     "\tADD A, C\n"
                     ".ELSE\n"
                     ".BGN\n"                     
                     "\tLD C, 0x00\n"
                     "\tLD A, 0x01\n"
                     "\tADD A, C\n"
                     ".END\n";

    string ifBlock = ".IFNDEF THE_UNDEFINED_SYMBOL\n"
                     ".BGN\n"     
                     "\tLD C, 0xFF\n"
                     "\tLD A, 0xFF\n"
                     "\tADD A, C\n"
                     ".ELSE\n"
                     ".BGN";
    
    string elseBlock = ".ELSE\n"
                     ".BGN\n"    
                     "\tLD C, 0x00\n"
                     "\tLD A, 0x01\n"
                     "\tADD A, C\n"
                     ".END";

    vector<string> symbolTable;
    symbolTable.push_back("THE_SYMBOL");

    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
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

TEST(AssemblerTests_ConditionalAssemblyPass, TestCodeRemovalIfNDef)
{
    string program = ".IFNDEF THE_SYMBOL\n"
                     ".BGN\n"     
                     "\tLD HL, 0x77FA\n"
                     "\tLD A, [HL]\n"
                     ".ELSE\n"
                     ".BGN\n"                     
                     "\tINC A\n"
                     "\tJP MY_LABEL\n"
                     ".END\n";
    
    string result =  "                  \n"
                     "    \n"     
                     "\tLD HL, 0x77FA\n"
                     "\tLD A, [HL]\n"
                     "     \n"
                     "    \n"                     
                     "      \n"
                     "            \n"
                     "    \n";
    
    string SecondPassResult =  "                  \n"
                     "    \n"     
                     "              \n"
                     "           \n"
                     "     \n"
                     "    \n"    
                     "\tINC A\n"
                     "\tJP MY_LABEL\n"
                     "    \n";

    vector<string> symbolTable;
    symbolTable.push_back("THE_OTHER_SYMBOL");

    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    pass->Process(program);
    auto processedCode = pass->Result();

    EXPECT_STREQ(result.c_str(), processedCode.c_str());
    
    symbolTable.push_back("THE_SYMBOL");

    auto secondPass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, stream);
    secondPass->Process(program);
    auto secondPassProcessedCode = secondPass->Result();

    EXPECT_STREQ(SecondPassResult.c_str(), secondPassProcessedCode.c_str());
}

TEST(AssemblerTests_ConditionalAssemblyPass, TestRemovalIfNDef2)
{
    string program = ".IFNDEF NO_DEFINED\n"
                     ".BGN\n"     
                     "\tSLA A, 1\n"
                     "\tLD C, A\n"
                     ".END\n";
    
    string result = "                  \n"
                     "    \n" 
                     "\tSLA A, 1\n"
                     "\tLD C, A\n"
                     "    \n";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    pass->Process(program);
    auto processedCode = pass->Result();

    EXPECT_STREQ(result.c_str(), processedCode.c_str());
}

TEST(AssemblerTests_ConditionalAssemblyPass, TestRemovalIfNDef3)
{
    string program = ".IFNDEF DEFINED\n"
                     ".BGN\n"     
                     "    \n"     
                     "\tSLA A, 1\n"
                     ".IFNDEF NO_DEFINED\n"
                     ".BGN\n"     
                     "    \n"                      
                     "\tLD C, A\n"
                     "\tLD C, A\n"
                     "\tLD C, A\n"
                     ".ELSE\n"
                     ".BGN\n"                     
                     "\tLD C, A\n"
                     ".END\n"
                     "\tSUB A, B\n"
                     ".END\n";
    
    string result = "               \n"
                     "    \n"     
                     "    \n"     
                     "\tSLA A, 1\n"
                     "                  \n"
                     "    \n"     
                     "    \n"                      
                     "        \n"
                     "        \n"
                     "        \n"
                     "     \n"
                     "    \n"                     
                     "\tLD C, A\n"
                     "    \n"
                     "\tSUB A, B\n"
                     "    \n";

    vector<string> symbolTable; 
    symbolTable.push_back("NO_DEFINED");
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    pass->Process(program);
    auto processedCode = pass->Result();

    EXPECT_STREQ(result.c_str(), processedCode.c_str());
}

TEST(AssemblerTests_ConditionalAssemblyPass, TestRemovalIfNDef4)
{
    string program = ".IFNDEF DEFINED\n"
                     ".BGN\n"     
                     "\tSLA A, 1\n"
                     ".IFNDEF NO_DEFINED\n"
                     ".BGN\n"                      
                     "\tLD C, A\n"
                     "\tLD C, A\n"
                     "\tLD C, A\n"
                     ".ELSE\n"
                     ".BGN\n"                     
                     "\tLD C, A\n"
                     ".END\n"
                     "\tSUB A, B\n"
                     ".ELSE\n"
                     ".BGN\n"                     
                     "\tSLA A, 1\n"
                     "\tLD C, A\n"
                     ".END\n";
    
    string result = "               \n"
                     "    \n"     
                     "\tSLA A, 1\n"
                     "                  \n"
                     "    \n"                      
                     "        \n"
                     "        \n"
                     "        \n"
                     "     \n"
                     "    \n"                     
                     "\tLD C, A\n"
                     "    \n"
                     "\tSUB A, B\n"
                     "     \n"
                     "    \n"                     
                     "         \n"
                     "        \n"
                     "    \n";

    vector<string> symbolTable; 
    symbolTable.push_back("NO_DEFINED");
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    pass->Process(program);
    auto processedCode = pass->Result();

    EXPECT_STREQ(result.c_str(), processedCode.c_str());
}

TEST(AssemblerTests_ConditionalAssemblyPass, SymbolDefintion)
{
    string program = ".DEF MY_SYMBOL\n";
    string output = "              \n";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    pass->Process(program);
    auto processedCode = pass->Result();

    EXPECT_EQ(1llu, symbolTable.size());
    EXPECT_STREQ("MY_SYMBOL", symbolTable[0].c_str());
    EXPECT_STREQ(output.c_str(), processedCode.c_str());
}

TEST(AssemblerTests_ConditionalAssemblyPass, MultipleSymbolDefintion)
{
    string program = ".DEF MY_SYMBOL_1\n"
                     ".DEF MY_SYMBOL_2\n"
                     ".DEF MY_SYMBOL_3\n"
                     ".DEF MY_SYMBOL_4\n"
                     ".DEF MY_SYMBOL_4\n"
                     "LD A, 0xFF";

    string output =  "                \n"
                     "                \n"
                     "                \n"
                     "                \n"
                     "                \n"
                     "LD A, 0xFF";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    pass->Process(program);
    auto processedCode = pass->Result();

    EXPECT_EQ(4llu, symbolTable.size());
    EXPECT_STREQ("MY_SYMBOL_1", symbolTable[0].c_str());
    EXPECT_STREQ("MY_SYMBOL_2", symbolTable[1].c_str());
    EXPECT_STREQ("MY_SYMBOL_3", symbolTable[2].c_str());
    EXPECT_STREQ("MY_SYMBOL_4", symbolTable[3].c_str());
    EXPECT_STREQ(output.c_str(), processedCode.c_str());
}

TEST(AssemblerTests_ConditionalAssemblyPass, MalformedDefDirective)
{
    string program = ".DEF\n";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);

    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Malformed '.DEF' directive (identifier expected)");
}

TEST(AssemblerTests_ConditionalAssemblyPass, MalformedDefDirective2)
{
    string program = ".DEF";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);

    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Malformed '.DEF' directive (identifier expected)");
}

TEST(AssemblerTests_ConditionalAssemblyPass, MalformedDefDirective3)
{
    string program = ".DEF              ";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);

    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Malformed '.DEF' directive (identifier expected)");
}

TEST(AssemblerTests_ConditionalAssemblyPass, MalformedIdentifier1)
{
    string program = ".DEF .DEF";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);

    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Invalid Pre-Assembly symbol identifier '.DEF'");
}

TEST(AssemblerTests_ConditionalAssemblyPass, MalformedIdentifier2)
{
    string program = ".DEF $$%AGGHJAYYS";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);

    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Invalid Pre-Assembly symbol identifier '$$%AGGHJAYYS'");
}

TEST(AssemblerTests_ConditionalAssemblyPass, MalformedIdentifier3)
{
    string program = ".DEF 12223";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);

    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Invalid Pre-Assembly symbol identifier '12223'");
}

TEST(AssemblerTests_ConditionalAssemblyPass, MalformedIdentifier4)
{
    string program = ".DEF 0xFFFF";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);

    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Invalid Pre-Assembly symbol identifier '0xFFFF'");
}

TEST(AssemblerTests_ConditionalAssemblyPass, MultipleStatementesInOneLine)
{
    string program = ".DEF MY_SYMBOL LD A, 0xFF\n";
    string output =  "               LD A, 0xFF\n";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    pass->Process(program);
    auto processedCode = pass->Result();

    EXPECT_EQ(1llu, symbolTable.size());
    EXPECT_STREQ("MY_SYMBOL", symbolTable[0].c_str());
    EXPECT_STREQ(output.c_str(), processedCode.c_str());
}

TEST(AssemblerTests_ConditionalAssemblyPass, SymbolUnDefintion)
{
    string program = ".UNDEF MY_SYMBOL\n";
    string output =  "                \n";

    vector<string> symbolTable; 
    symbolTable.push_back("MY_SYMBOL");
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    pass->Process(program);
    auto processedCode = pass->Result();

    EXPECT_EQ(0llu, symbolTable.size());
    EXPECT_STREQ(output.c_str(), processedCode.c_str());
}

TEST(AssemblerTests_ConditionalAssemblyPass, SymbolUnDefintion1)
{
    string program1 = ".DEF MY_SYMBOL\n";
    string program2 = ".UNDEF MY_SYMBOL\n";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    pass->Process(program1);

    EXPECT_EQ(1llu, symbolTable.size());
    EXPECT_STREQ("MY_SYMBOL", symbolTable[0].c_str());

    pass->Process(program2);
    EXPECT_EQ(0llu, symbolTable.size());
}

TEST(AssemblerTests_ConditionalAssemblyPass, SymbolUnDefintion2)
{
    string program1 = ".DEF MY_SYMBOL\n"
                      ".DEF MY_SYMBOL_1\n"
                      ".DEF MY_SYMBOL_2\n";
    string program2 = ".UNDEF MY_SYMBOL\n";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    pass->Process(program1);

    EXPECT_EQ(3llu, symbolTable.size());
    EXPECT_STREQ("MY_SYMBOL", symbolTable[0].c_str());
    EXPECT_STREQ("MY_SYMBOL_1", symbolTable[1].c_str());
    EXPECT_STREQ("MY_SYMBOL_2", symbolTable[2].c_str());

    pass->Process(program2);
    EXPECT_EQ(2llu, symbolTable.size());
    EXPECT_STREQ("MY_SYMBOL_1", symbolTable[0].c_str());
    EXPECT_STREQ("MY_SYMBOL_2", symbolTable[1].c_str());
}

TEST(AssemblerTests_ConditionalAssemblyPass, SymbolUnDefintion3)
{
    string program1 = ".DEF MY_SYMBOL\n"
                      ".DEF MY_SYMBOL_1\n"
                      ".DEF MY_SYMBOL_2\n";
    string program2 = ".UNDEF MY_SYMBOL_2\n";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    pass->Process(program1);

    EXPECT_EQ(3llu, symbolTable.size());
    EXPECT_STREQ("MY_SYMBOL", symbolTable[0].c_str());
    EXPECT_STREQ("MY_SYMBOL_1", symbolTable[1].c_str());
    EXPECT_STREQ("MY_SYMBOL_2", symbolTable[2].c_str());

    pass->Process(program2);
    EXPECT_EQ(2llu, symbolTable.size());
    EXPECT_STREQ("MY_SYMBOL", symbolTable[0].c_str());
    EXPECT_STREQ("MY_SYMBOL_1", symbolTable[1].c_str());
}

TEST(AssemblerTests_ConditionalAssemblyPass, SymbolUnDefintion4)
{
    string program1 = ".DEF MY_SYMBOL\n"
                      ".DEF MY_SYMBOL_1\n"
                      ".DEF MY_SYMBOL_2\n";
    string program2 = ".UNDEF MY_SYMBOL_1\n";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    pass->Process(program1);

    EXPECT_EQ(3llu, symbolTable.size());
    EXPECT_STREQ("MY_SYMBOL", symbolTable[0].c_str());
    EXPECT_STREQ("MY_SYMBOL_1", symbolTable[1].c_str());
    EXPECT_STREQ("MY_SYMBOL_2", symbolTable[2].c_str());

    pass->Process(program2);
    EXPECT_EQ(2llu, symbolTable.size());
    EXPECT_STREQ("MY_SYMBOL", symbolTable[0].c_str());
    EXPECT_STREQ("MY_SYMBOL_2", symbolTable[1].c_str());
}

TEST(AssemblerTests_ConditionalAssemblyPass, SymbolUnDefintion5)
{
    string program1 = ".DEF MY_SYMBOL\n"
                      ".DEF MY_SYMBOL_1\n"
                      ".DEF MY_SYMBOL_2\n";
    string program2 = ".UNDEF MY_SYMBOL\n"
                      ".UNDEF MY_SYMBOL_1\n"
                      ".UNDEF MY_SYMBOL_2\n";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    pass->Process(program1);

    EXPECT_EQ(3llu, symbolTable.size());
    EXPECT_STREQ("MY_SYMBOL", symbolTable[0].c_str());
    EXPECT_STREQ("MY_SYMBOL_1", symbolTable[1].c_str());
    EXPECT_STREQ("MY_SYMBOL_2", symbolTable[2].c_str());

    pass->Process(program2);
    EXPECT_EQ(0llu, symbolTable.size());
}

TEST(AssemblerTests_ConditionalAssemblyPass, SymbolUnDefintionOfNotDefinedIdentifier)
{
    string program1 = ".UNDEF MY_SYMBOL\n";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);

    EXPECT_CALL((*stream), Write("Undefinition of non-defined pre-assembly symbol 'MY_SYMBOL'", ::_, ::_, ::_));
    pass->Process(program1);
    EXPECT_EQ(0llu, symbolTable.size());
}


TEST(AssemblerTests_ConditionalAssemblyPass, MalformedUnDefDirective)
{
    string program = ".UNDEF\n";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);

    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Malformed '.UNDEF' directive (identifier expected)");
}

TEST(AssemblerTests_ConditionalAssemblyPass, MalformedUnDefDirective2)
{
    string program = ".UNDEF";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);

    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Malformed '.UNDEF' directive (identifier expected)");
}

TEST(AssemblerTests_ConditionalAssemblyPass, MalformedUnDefDirective3)
{
    string program = ".UNDEF              ";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);

    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Malformed '.UNDEF' directive (identifier expected)");
}

TEST(AssemblerTests_ConditionalAssemblyPass, MalformedUnDefIdentifier1)
{
    string program = ".UNDEF .IFDEF";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);

    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Invalid Pre-Assembly symbol identifier '.IFDEF'");
}

TEST(AssemblerTests_ConditionalAssemblyPass, MalformedUnDefIdentifier2)
{
    string program = ".UNDEF &&*()()(**)";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);

    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Invalid Pre-Assembly symbol identifier '&&*()()(**)'");
}

TEST(AssemblerTests_ConditionalAssemblyPass, MalformedUnDefIdentifier3)
{
    string program = ".UNDEF 6789";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);

    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Invalid Pre-Assembly symbol identifier '6789'");
}

TEST(AssemblerTests_ConditionalAssemblyPass, MalformedUnDefIdentifier4)
{
    string program = ".UNDEF 0o3344";

    vector<string> symbolTable; 
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);

    ASSERT_EXCEPTION( { pass->Process(program); }, 
                      PreProcessorException, 
                      "Invalid Pre-Assembly symbol identifier '0o3344'");
}

TEST(AssemblerTests_ConditionalAssemblyPass, MultipleStatementesInOneLineWithUnDef)
{
    string program = ".UNDEF MY_SYMBOL LD B, 0x5A\n";
    string output =  "                 LD B, 0x5A\n";

    vector<string> symbolTable; 
    symbolTable.push_back("MY_SYMBOL");
    auto stream = make_shared<StreamMock>();
    auto streamMock = static_pointer_cast<MessageStream>(stream);
    auto pass = make_shared<ConditionalAssemblyPassWrapper>(symbolTable, streamMock);
    pass->Process(program);
    auto processedCode = pass->Result();

    EXPECT_EQ(0llu, symbolTable.size());
    EXPECT_STREQ(output.c_str(), processedCode.c_str());
}