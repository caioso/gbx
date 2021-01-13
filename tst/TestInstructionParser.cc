#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <string>
#include <vector>

#include "../src/InstructionParser.h"
#include "../src/Tokenizer.h"

using namespace gbxasm;
using namespace std;

class InstructionParserDecorator : public InstructionParser
{
public:
    InstructionParserDecorator(vector<Token>& tokens) : InstructionParser(tokens)
    {}

    vector<Symbol>& GetSymbolTable()
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
