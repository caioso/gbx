#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <string>
#include <vector>

#include "../src/InstructionParser.h"
#include "../src/Tokenizer.h"
#include "../src/symbols/Label.h"

using namespace gbxasm;
using namespace gbxasm::symbols;
using namespace std;

class InstructionParserDecorator : public InstructionParser
{
public:
    InstructionParserDecorator(vector<Token>& tokens) : InstructionParser(tokens)
    {}

    vector<SymbolWrapper>& GetSymbolTable()
    {
        return _symbolTable;
    }

    vector<Token>& GetTokens()
    {
        return _tokens;
    }
};

TEST(TestInstructionParser, Construction)
{
    auto tokenizer = make_shared<Tokenizer>();
    auto parser = make_shared<InstructionParser>(tokenizer->Tokens());
}

TEST(TestInstructionParser, StorageOfParsedTokens)
{
    string assembly = 
    "label:\n"
    "\tLD A, #0x00\n"
    "\tINC A\n"
    "\tLD HL, #FFAA\n"
    "\tLD (HL), A\n";

    auto tokenizer = make_shared<Tokenizer>();
         tokenizer->ToToken(assembly);
    auto parser = make_shared<InstructionParserDecorator>(tokenizer->Tokens());

    EXPECT_EQ(static_cast<size_t>(12), parser->GetTokens().size());
}

TEST(TestInstructionParser, DetectLabel)
{
    string assembly = 
    "label:\n"
    "label2:\n"
    "label3:\n";

    auto tokenizer = make_shared<Tokenizer>();
         tokenizer->ToToken(assembly);
    auto parser = make_shared<InstructionParserDecorator>(tokenizer->Tokens());
         parser->ParseTokens();

    EXPECT_EQ(static_cast<size_t>(3), parser->GetSymbolTable().size());
}

TEST(TestInstructionParser, ParseLabels)
{
    string assembly = 
    "label:\n"
    "label2:\n"
    "label3:\n";

    auto tokenizer = make_shared<Tokenizer>();
         tokenizer->ToToken(assembly);
    auto parser = make_shared<InstructionParserDecorator>(tokenizer->Tokens());
         parser->ParseTokens();

    auto metadataReference = static_pointer_cast<Label>(parser->GetSymbolTable()[0].Symbol);
    EXPECT_FALSE(metadataReference == nullptr);
    EXPECT_STREQ("label", metadataReference->Identifier().c_str());
    EXPECT_STREQ("label:", metadataReference->RawToken().c_str());
    EXPECT_EQ(static_cast<size_t>(1), metadataReference->Line());
    EXPECT_EQ(static_cast<size_t>(1), metadataReference->Column());

    metadataReference = static_pointer_cast<Label>(parser->GetSymbolTable()[1].Symbol);
    EXPECT_FALSE(metadataReference == nullptr);
    EXPECT_STREQ("label2", metadataReference->Identifier().c_str());
    EXPECT_STREQ("label2:", metadataReference->RawToken().c_str());
    EXPECT_EQ(static_cast<size_t>(2), metadataReference->Line());
    EXPECT_EQ(static_cast<size_t>(1), metadataReference->Column());

    metadataReference = static_pointer_cast<Label>(parser->GetSymbolTable()[2].Symbol);
    EXPECT_FALSE(metadataReference == nullptr);
    EXPECT_STREQ("label3", metadataReference->Identifier().c_str());
    EXPECT_STREQ("label3:", metadataReference->RawToken().c_str());
    EXPECT_EQ(static_cast<size_t>(3), metadataReference->Line());
    EXPECT_EQ(static_cast<size_t>(1), metadataReference->Column());
}

TEST(TestInstructionParser, ParseLabelsWithExtraSpace)
{
    string assembly = 
    "      labelA:               \n"
    "         \t       labelB: "
    "    \r    labelC:\n                ";

    auto tokenizer = make_shared<Tokenizer>();
         tokenizer->ToToken(assembly);
    auto parser = make_shared<InstructionParserDecorator>(tokenizer->Tokens());
         parser->ParseTokens();

    auto metadataReference = static_pointer_cast<Label>(parser->GetSymbolTable()[0].Symbol);
    EXPECT_FALSE(metadataReference == nullptr);
    EXPECT_STREQ("labelA", metadataReference->Identifier().c_str());
    EXPECT_STREQ("labelA:", metadataReference->RawToken().c_str());
    EXPECT_EQ(static_cast<size_t>(1), metadataReference->Line());
    EXPECT_EQ(static_cast<size_t>(7), metadataReference->Column());

    metadataReference = static_pointer_cast<Label>(parser->GetSymbolTable()[1].Symbol);
    EXPECT_FALSE(metadataReference == nullptr);
    EXPECT_STREQ("labelB", metadataReference->Identifier().c_str());
    EXPECT_STREQ("labelB:", metadataReference->RawToken().c_str());
    EXPECT_EQ(static_cast<size_t>(2), metadataReference->Line());
    EXPECT_EQ(static_cast<size_t>(18), metadataReference->Column());

    metadataReference = static_pointer_cast<Label>(parser->GetSymbolTable()[2].Symbol);
    EXPECT_FALSE(metadataReference == nullptr);
    EXPECT_STREQ("labelC", metadataReference->Identifier().c_str());
    EXPECT_STREQ("labelC:", metadataReference->RawToken().c_str());
    EXPECT_EQ(static_cast<size_t>(2), metadataReference->Line());
    EXPECT_EQ(static_cast<size_t>(35), metadataReference->Column());
}
