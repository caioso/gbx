#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <string>
#include <vector>

#include "../src/InstructionParser.h"
#include "../src/Tokenizer.h"
//#include "../src/symbols/Number.h"

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

TEST(TestNumberSymbol, DecimalNumberParsing)
{
}
